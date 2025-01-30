#include "ShaderDataType.h"

namespace Bloodshot
{
	uint8_t GetShaderDataTypeSize(const EShaderDataType Type)
	{
		switch (Type)
		{
			case EShaderDataType::Bool: return 1;
			case EShaderDataType::Int: return 4;
			case EShaderDataType::Int2: return 4 * 2;
			case EShaderDataType::Int3: return 4 * 3;
			case EShaderDataType::Int4: return 4 * 4;
			case EShaderDataType::Float: return 4;
			case EShaderDataType::Float2: return 4 * 2;
			case EShaderDataType::Float3: return 4 * 3;
			case EShaderDataType::Float4: return 4 * 4;
			case EShaderDataType::Mat3: return 4 * 3 * 3;
			case EShaderDataType::Mat4: return 4 * 4 * 4;
		}

		BS_ASSERT(false, "Trying to get size from unknown ShaderDataType");
		return 0;
	}

	uint8_t GetShaderDataTypeElementCount(const EShaderDataType Type)
	{
		switch (Type)
		{
			case EShaderDataType::Bool: return 1;
			case EShaderDataType::Int: return 1;
			case EShaderDataType::Int2: return 2;
			case EShaderDataType::Int3: return 3;
			case EShaderDataType::Int4: return 4;
			case EShaderDataType::Float: return 1;
			case EShaderDataType::Float2: return 2;
			case EShaderDataType::Float3: return 3;
			case EShaderDataType::Float4: return 4;
			case EShaderDataType::Mat3: return 3; // 3* float3
			case EShaderDataType::Mat4: return 4; // 4* float4
		}

		BS_ASSERT(false, "Trying to get element count from unknown ShaderDataType");
		return 0;
	}

	GLenum ShaderDataTypeToOpenGLBaseType(const EShaderDataType Type)
	{
		switch (Type)
		{
			case EShaderDataType::Bool: return GL_BOOL;
			case EShaderDataType::Int: return GL_INT;
			case EShaderDataType::Int2: return GL_INT;
			case EShaderDataType::Int3: return GL_INT;
			case EShaderDataType::Int4: return GL_INT;
			case EShaderDataType::Float: return GL_FLOAT;
			case EShaderDataType::Float2: return GL_FLOAT;
			case EShaderDataType::Float3: return GL_FLOAT;
			case EShaderDataType::Float4: return GL_FLOAT;
			case EShaderDataType::Mat3: return GL_FLOAT;
			case EShaderDataType::Mat4: return GL_FLOAT;
		}

		BS_ASSERT(false, "Trying to convert unknown ShaderDataType to OpenGLBaseType");
		return 0;
	}
}
