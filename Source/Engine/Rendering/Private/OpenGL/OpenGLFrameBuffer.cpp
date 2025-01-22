#include "OpenGL/OpenGLFrameBuffer.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	namespace Private
	{
		static GLenum GetTextureTarget(const bool bMultisampled)
		{
			return bMultisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(const bool bMultisampled, uint32_t* OutTextures, const uint32_t Count)
		{
			glCreateTextures(GetTextureTarget(bMultisampled), Count, OutTextures);
		}

		static void BindTexture(const bool bMultisampled, uint32_t TextureID)
		{
			glBindTexture(GetTextureTarget(bMultisampled), TextureID);
		}

		static void AttachColorTexture(const uint32_t TextureID,
			const uint32_t SampleCount,
			const GLenum InternalFormat,
			const GLenum Format,
			const uint32_t Width,
			const uint32_t Height,
			const uint32_t Index)
		{
			const bool bMultisampled = SampleCount > 1;

			if (bMultisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleCount, InternalFormat, Width, Height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, Width, Height, 0, Format, GL_UNSIGNED_BYTE, nullptr);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + Index, GetTextureTarget(bMultisampled), TextureID, 0);
		}

		static void AttachDepthTexture(const uint32_t TextureID,
			const uint32_t SampleCount,
			const GLenum Format,
			const GLenum AttachmentType,
			const uint32_t Width,
			const uint32_t Height)
		{
			const bool bMultisampled = SampleCount > 1;

			if (bMultisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleCount, Format, Width, Height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, Format, Width, Height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, AttachmentType, GetTextureTarget(bMultisampled), TextureID, 0);
		}

		static GLenum FrameBufferTextureFormatToOpenGLTextureFormat(const EFrameBufferTextureFormat Format)
		{
			switch (Format)
			{
				case Bloodshot::EFrameBufferTextureFormat::RGBA8:return GL_RGBA8;
				case Bloodshot::EFrameBufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			BS_ASSERT(false, "Unknown FrameBufferTextureFormat passed, or target FrameBufferTextureFormat cannot be converted to OpenGLTextureFormat");
			return 0;
		}
	}

	FOpenGLFrameBuffer::FOpenGLFrameBuffer(const FFrameBufferSettings& Settings)
		: IFrameBuffer(Settings)
	{
		for (EFrameBufferTextureFormat Attachment : Settings.Attachments)
		{
			if (Attachment != EFrameBufferTextureFormat::DEPTH24_STENCIL8)
			{
				ColorAttachments.push_back(Attachment);
			}
			else
			{
				DepthAttachment = Attachment;
			}
		}

		Invalidate();
	}

	FOpenGLFrameBuffer::~FOpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &UniqueID);
		glDeleteTextures((GLsizei)ColorAttachmentIndices.size(), ColorAttachmentIndices.data());
		glDeleteTextures(1, &DepthAttachmentIndex);
	}

	void FOpenGLFrameBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, UniqueID);
		glViewport(0, 0, Settings.Width, Settings.Height);
	}

	void FOpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void FOpenGLFrameBuffer::Resize(const uint32_t Width, const uint32_t Height)
	{
		if (Width == 0 || Height == 0 || Width > MaxSize || Height > MaxSize)
		{
			BS_LOG(Warning, "Attempting to resize FrameBuffer to {0}, {1}", Width, Height);
			return;
		}

		Settings.Width = Width;
		Settings.Height = Height;

		Invalidate();
	}

	int FOpenGLFrameBuffer::ReadPixel(const uint32_t AttachmentIndex, const uint32_t X, const uint32_t Y)
	{
		BS_ASSERT(AttachmentIndex < ColorAttachmentIndices.size(), "AttachmentIndex was out of range of Color Attachments");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + AttachmentIndex);

		int PixelData;

		glReadPixels(X, Y, 1, 1, GL_RED_INTEGER, GL_INT, &PixelData);

		return PixelData;
	}

	void FOpenGLFrameBuffer::ClearAttachment(const uint32_t AttachmentIndex, const uint32_t Value)
	{
		BS_ASSERT(AttachmentIndex < ColorAttachmentIndices.size(), "AttachmentIndex was out of range of Color Attachments");

		EFrameBufferTextureFormat Attachment = ColorAttachments[AttachmentIndex];

		glClearTexImage(ColorAttachmentIndices[AttachmentIndex],
			0,
			Private::FrameBufferTextureFormatToOpenGLTextureFormat(Attachment),
			GL_INT,
			&Value);
	}

	void FOpenGLFrameBuffer::Invalidate()
	{
		if (UniqueID)
		{
			glDeleteFramebuffers(1, &UniqueID);
			glDeleteTextures((GLsizei)ColorAttachmentIndices.size(), ColorAttachmentIndices.data());
			glDeleteTextures(1, &DepthAttachmentIndex);

			ColorAttachmentIndices.clear();
			DepthAttachmentIndex = 0;
		}

		glCreateFramebuffers(1, &UniqueID);
		glBindFramebuffer(GL_FRAMEBUFFER, UniqueID);

		const bool bMultisampled = Settings.Samples > 1;

		const size_t ColorAttachmentsSize = ColorAttachments.size();

		if (ColorAttachmentsSize)
		{
			ColorAttachmentIndices.resize(ColorAttachmentsSize);
			Private::CreateTextures(bMultisampled, ColorAttachmentIndices.data(), (uint32_t)ColorAttachmentsSize);

			for (size_t i = 0; i < ColorAttachmentsSize; ++i)
			{
				Private::BindTexture(bMultisampled, ColorAttachmentIndices[i]);
				switch (ColorAttachments[i])
				{
					case EFrameBufferTextureFormat::RGBA8:
					{
						Private::AttachColorTexture(ColorAttachmentIndices[i],
							Settings.Samples,
							GL_RGBA8,
							GL_RGBA,
							Settings.Width,
							Settings.Height,
							(uint32_t)i);

						break;
					}
					case EFrameBufferTextureFormat::RED_INTEGER:
					{
						Private::AttachColorTexture(ColorAttachmentIndices[i],
							Settings.Samples,
							GL_R32I,
							GL_RED_INTEGER,
							Settings.Width,
							Settings.Height,
							(uint32_t)i);

						break;
					}
				}
			}
		}

		if (DepthAttachment != EFrameBufferTextureFormat::None)
		{
			Private::CreateTextures(bMultisampled, &DepthAttachmentIndex, 1);
			Private::BindTexture(bMultisampled, DepthAttachmentIndex);

			switch (DepthAttachment)
			{
				case EFrameBufferTextureFormat::DEPTH24_STENCIL8:
				{
					Private::AttachDepthTexture(DepthAttachmentIndex,
						Settings.Samples,
						GL_DEPTH24_STENCIL8,
						GL_DEPTH_ATTACHMENT,
						Settings.Width,
						Settings.Height);

					break;
				}
			}
		}

		if (ColorAttachmentIndices.size() > 1)
		{
			BS_ASSERT(ColorAttachmentIndices.size() <= 4, "Not enough Color Attachments");

			GLenum Buffers[4] =
			{
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
			};

			glDrawBuffers((GLsizei)ColorAttachmentIndices.size(), Buffers);
		}
		else if (ColorAttachmentIndices.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		BS_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FrameBuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
