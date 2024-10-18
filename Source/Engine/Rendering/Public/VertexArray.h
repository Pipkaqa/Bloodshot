#pragma once

#include "IndexBuffer.h"
#include "Platform.h"
#include "VertexBuffer.h"

#include <cstdint>
#include <vector>

namespace Bloodshot
{
	class IVertexArray abstract
	{
	public:
		virtual ~IVertexArray() {};

		NODISCARD FORCEINLINE const std::vector<IVertexBuffer*>& GetVertexBuffers() const
		{
			return VertexBuffers;
		}

		NODISCARD FORCEINLINE const IIndexBuffer* GetIndexBuffer() const
		{
			return IndexBuffer;
		}

		NODISCARD FORCEINLINE uint32_t GetVertexCount() const
		{
			return VertexCount;
		}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(IVertexBuffer* const VertexBuffer) = 0;
		virtual void SetIndexBuffer(IIndexBuffer* const IndexBuffer) = 0;

	protected:
		std::vector<IVertexBuffer*> VertexBuffers;
		IIndexBuffer* IndexBuffer = nullptr;
		uint32_t VertexBufferIndex = 0;
		uint32_t VertexCount = 0;
	};
}
