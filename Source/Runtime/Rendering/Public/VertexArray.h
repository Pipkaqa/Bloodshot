#pragma once

#include "Core.h"

#include "IndexBuffer.h"
#include "VertexBuffer.h"

namespace Bloodshot
{
	class IVertexArray
	{
	public:
		using FVertexBufferArray = TArray<TUniquePtr<IVertexBuffer>>;

		virtual ~IVertexArray() {};

		NODISCARD FORCEINLINE const FVertexBufferArray& GetVertexBuffers() const
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
		FVertexBufferArray VertexBuffers;
		TUniquePtr<IIndexBuffer> IndexBuffer = nullptr;
		uint32_t VertexBufferIndex = 0;
		uint32_t VertexCount = 0;
	};
}
