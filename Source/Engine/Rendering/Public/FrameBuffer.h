#pragma once

#include <vector>
#include <cstdint>

namespace Bloodshot
{
	enum class EFrameBufferTextureFormat : uint8_t
	{
		None = 0,
		RGBA8,
		RED_INTEGER,
		DEPTH24_STENCIL8,
		Depth = DEPTH24_STENCIL8
	};

	struct FFrameBufferSettings final
	{
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;

		std::vector<EFrameBufferTextureFormat> Attachments;
	};

	class IFrameBuffer abstract
	{
	public:
		IFrameBuffer(const FFrameBufferSettings& Settings);
		virtual ~IFrameBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		
		virtual void Resize(const uint32_t Width, const uint32_t Height) = 0;
		virtual int ReadPixel(const uint32_t AttachmentIndex, const uint32_t X, const uint32_t Y) = 0;

		virtual void ClearAttachment(const uint32_t AttachmentIndex, const uint32_t Value) = 0;

	protected:
		static constexpr inline uint32_t MaxSize = 8192;

		FFrameBufferSettings Settings;

		std::vector<EFrameBufferTextureFormat> ColorAttachments;
		EFrameBufferTextureFormat DepthAttachment = EFrameBufferTextureFormat::None;

		std::vector<uint32_t> ColorAttachmentIndices;
		uint32_t DepthAttachmentIndex = 0;
	};
}
