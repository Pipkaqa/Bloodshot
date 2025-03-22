#include "OpenGL/OpenGLTexture.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FOpenGLTexture::FOpenGLTexture(const uint32_t Width,
		const uint32_t Height,
		const EColorChannels Channels,
		const void* Data)
		: ITexture(Width, Height, Channels)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &UniqueID);

		switch (Channels)
		{
			case EColorChannels::R:
			{
				glTextureStorage2D(UniqueID, 1, GL_R8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RED, GL_UNSIGNED_BYTE, Data);
				break;
			}
			case EColorChannels::RG:
			{
				glTextureStorage2D(UniqueID, 1, GL_RG8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RG, GL_UNSIGNED_BYTE, Data);
				break;
			}
			case EColorChannels::RGB:
			{
				glTextureStorage2D(UniqueID, 1, GL_RGB8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RGB, GL_UNSIGNED_BYTE, Data);
				break;
			}
			case EColorChannels::RGBA:
			{
				glTextureStorage2D(UniqueID, 1, GL_RGBA8, Width, Height);
				glTextureSubImage2D(UniqueID, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Data);
				break;
			}
		}

		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateTextureMipmap(UniqueID);
	}

	FOpenGLTexture::~FOpenGLTexture()
	{
		glDeleteTextures(1, &UniqueID);
	}

	void FOpenGLTexture::Bind(const ETextureUnit Unit) const
	{
		glBindTextureUnit(static_cast<uint8_t>(Unit), UniqueID);
	}

	void FOpenGLTexture::Unbind(const ETextureUnit Unit) const
	{
		glBindTextureUnit(static_cast<uint8_t>(Unit), 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
