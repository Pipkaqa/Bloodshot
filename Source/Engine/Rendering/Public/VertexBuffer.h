#pragma once

#include "VertexBufferLayout.h"

#include <cstdint>

namespace Bloodshot
{
	class IVertexBuffer abstract
	{
	public:
		IVertexBuffer(const uint32_t VertexCount);
		virtual ~IVertexBuffer() {}

		virtual const FVertexBufferLayout& GetLayout() const = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* Data, const uint32_t Size) = 0;
		virtual void SetLayout(const FVertexBufferLayout& Layout) = 0;

		NODISCARD FORCEINLINE uint32_t GetVertexCount() const
		{
			return VertexCount;
		}

	private:
		uint32_t VertexCount;
	};
}
