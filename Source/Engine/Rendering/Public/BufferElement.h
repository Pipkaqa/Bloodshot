#pragma once

#include "Core.h"

#include "ShaderDataType.h"

namespace Bloodshot
{
	struct FVertexBufferElement final
	{
		FVertexBufferElement() {}
		FVertexBufferElement(const EShaderDataType Type, FStringView Name, const bool bNormalized = false);

		EShaderDataType Type = EShaderDataType::None;
		FString Name = "";

		uint16_t Size = 0;
		uint16_t Offset = 0;

		bool bNormalized = false;
	};
}
