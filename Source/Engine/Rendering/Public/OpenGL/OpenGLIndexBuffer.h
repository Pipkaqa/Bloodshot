#pragma once

#include "IndexBuffer.h"
#include "Platform.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class FOpenGLIndexBuffer final : public IIndexBuffer
	{
	public:
		FOpenGLIndexBuffer(const uint32_t Count, const uint32_t* const Indices);
		virtual ~FOpenGLIndexBuffer() override;

		NODISCARD virtual uint32_t GetIndexCount() const override;
		
		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		uint32_t UniqueID = 0;
		uint32_t IndexCount = 0;
	};
}
