#include "VertexBufferLayout.h"

namespace Bloodshot
{
	FVertexBufferLayout::FVertexBufferLayout(std::initializer_list<FVertexBufferElement> Elements)
		: ElementVec(Elements)
	{
		CalculateElementsOffsetsAndStride();
	}

	

	void FVertexBufferLayout::CalculateElementsOffsetsAndStride()
	{
		uint16_t Offset = 0;
		Stride = 0;

		for (FVertexBufferElement& Element : ElementVec)
		{
			Element.Offset = Offset;
			Offset += Element.Size;
			Stride += Element.Size;
		}
	}
}
