#pragma once

#include "IndexBuffer.h"
#include "Platform/Platform.h"
#include "VertexBuffer.h"
#include "Templates/SmartPointers.h"

#include <cstdint>
#include <vector>

namespace Bloodshot
{
	class IVertexArray
	{
	public:
		virtual ~IVertexArray() {};

		NODISCARD FORCEINLINE const TVector<TUniquePtr<IVertexBuffer>>& GetVertexBuffers() const
		{
			return VertexBuffers;
		}

		NODISCARD FORCEINLINE const TUniquePtr<IIndexBuffer>& GetIndexBuffer() const
		{
			return IndexBuffer;
		}

		NODISCARD FORCEINLINE uint32_t GetVertexCount() const
		{
			return VertexCount;
		}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(TUniquePtr<IVertexBuffer>&& VertexBuffer) = 0;
		virtual void SetIndexBuffer(TUniquePtr<IIndexBuffer>&& IndexBuffer) = 0;

	protected:
		TVector<TUniquePtr<IVertexBuffer>> VertexBuffers;
		TUniquePtr<IIndexBuffer> IndexBuffer = nullptr;
		uint32_t VertexBufferIndex = 0;
		uint32_t VertexCount = 0;
	};
}
