#pragma once

#include "Core.h"

#include "BufferElement.h"

namespace Bloodshot
{
	class FVertexBufferLayout final
	{
	public:
		using FElementArray = TArray<FVertexBufferElement>;

		FORCEINLINE FVertexBufferLayout() noexcept = default;
		FVertexBufferLayout(std::initializer_list<FVertexBufferElement> Elements);

		NODISCARD FORCEINLINE const FElementArray& GetElements() const
		{
			return Elements;
		}

		NODISCARD FORCEINLINE uint16_t GetStride() const
		{
			return Stride;
		}

		NODISCARD FORCEINLINE FElementArray::FIterator CreateIterator()
		{
			return Elements.CreateIterator();
		}

		NODISCARD FORCEINLINE FElementArray::FConstIterator CreateConstIterator() const
		{
			return Elements.CreateConstIterator();
		}

	private:
		FElementArray Elements;
		uint16_t Stride = 0;

		void CalculateElementsOffsetsAndStride();
	};
}

