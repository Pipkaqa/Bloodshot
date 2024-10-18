#pragma once

#include "MathLibrary.h"
#include "Platform.h"
#include "Templates/TypeInfo.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include <cstdint>

namespace Bloodshot
{
	class FOpenGLVertexBuffer final : public IVertexBuffer
	{
	public:
		FOpenGLVertexBuffer(const void* const Data, const uint32_t Size, const uint32_t VertexCount);
		virtual ~FOpenGLVertexBuffer() override;

		NODISCARD FORCEINLINE InstanceID_t GetInstanceID() const
		{
			return UniqueID;
		}

		NODISCARD FORCEINLINE virtual const FVertexBufferLayout& GetLayout() const override
		{
			return Layout;
		}

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* Data, const uint32_t Size) override;
		virtual void SetLayout(const FVertexBufferLayout& Layout) override;

	private:
		uint32_t UniqueID = 0;
		FVertexBufferLayout Layout = {};
	};
}
