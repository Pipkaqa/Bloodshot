#pragma once

#include "OpenGL/OpenGLHeader.h"
#include "Platform.h"

#include <cstdint>

namespace Bloodshot
{
	enum class EShaderDataType : uint8_t
	{
		None = 0,
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4
	};

	NODISCARD uint8_t GetShaderDataTypeSize(const EShaderDataType Type);
	NODISCARD uint8_t GetShaderDataTypeElementCount(const EShaderDataType Type);
	NODISCARD GLenum ShaderDataTypeToOpenGLBaseType(const EShaderDataType Type);
}
