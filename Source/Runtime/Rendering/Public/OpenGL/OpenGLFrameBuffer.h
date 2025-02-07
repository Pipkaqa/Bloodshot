#pragma once

#include "FrameBuffer.h"

namespace Bloodshot
{
	class FOpenGLFrameBuffer final : public IFrameBuffer
	{
	public:
		FOpenGLFrameBuffer(const FFrameBufferSettings& Settings);
		virtual ~FOpenGLFrameBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Resize(const uint32_t Width, const uint32_t Height) override;
		virtual int ReadPixel(const uint32_t AttachmentIndex, const uint32_t X, const uint32_t Y) override;

		virtual void ClearAttachment(const uint32_t AttachmentIndex, const uint32_t Value) override;

		void Invalidate();

	private:
		uint32_t UniqueID = 0;
	};
}
