#pragma once

#include "BufferElement.h"
#include "Platform/Platform.h"

#include <initializer_list>
#include <vector>

namespace Bloodshot
{
	class FVertexBufferLayout final
	{
	public:
		FVertexBufferLayout() {}
		FVertexBufferLayout(std::initializer_list<FVertexBufferElement> Elements);

		NODISCARD FORCEINLINE const std::vector<FVertexBufferElement>& GetElements() const
		{
			return ElementVec;
		}

		NODISCARD FORCEINLINE uint16_t GetStride() const
		{
			return Stride;
		}

		NODISCARD FORCEINLINE std::vector<FVertexBufferElement>::iterator GetBeginIt()
		{
			return ElementVec.begin();
		}

		NODISCARD FORCEINLINE std::vector<FVertexBufferElement>::iterator GetEndIt()
		{
			return ElementVec.end();
		}

		NODISCARD FORCEINLINE std::vector<FVertexBufferElement>::const_iterator GetConstBeginIt() const
		{
			return ElementVec.cbegin();
		}

		NODISCARD FORCEINLINE std::vector<FVertexBufferElement>::const_iterator GetConstEndIt() const
		{
			return ElementVec.cend();
		}

	private:
		std::vector<FVertexBufferElement> ElementVec;
		uint16_t Stride = 0;

		void CalculateElementsOffsetsAndStride();
	};
}

