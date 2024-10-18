#pragma once

#include "AssertionMacros.h"
#include "ShaderDataType.h"

#include <string>

namespace Bloodshot
{
	struct FVertexBufferElement final
	{
		FVertexBufferElement() {}
		FVertexBufferElement(const EShaderDataType Type, std::string_view Name, const bool bNormalized = false);

		EShaderDataType Type = EShaderDataType::None;
		std::string Name = "";

		uint16_t Size = 0;
		uint16_t Offset = 0;

		bool bNormalized = false;
	};
}
