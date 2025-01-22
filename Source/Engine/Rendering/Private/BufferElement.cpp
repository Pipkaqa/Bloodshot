#include "BufferElement.h"

namespace Bloodshot
{
	FVertexBufferElement::FVertexBufferElement(const EShaderDataType Type, FStringView Name, const bool bNormalized)
		: Name(Name)
		, Type(Type)
		, Size(GetShaderDataTypeSize(Type))
		, bNormalized(bNormalized)
	{
	}
}
