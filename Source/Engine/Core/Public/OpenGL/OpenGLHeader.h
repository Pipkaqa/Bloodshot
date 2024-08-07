#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Platform.h"

namespace Bloodshot
{
	NODISCARD FORCEINLINE constexpr const char* OpenGLShaderTypeToString(const GLenum Type)
	{
		switch (Type)
		{
			case GL_VERTEX_SHADER: return "Vertex";
			case GL_FRAGMENT_SHADER: return "Fragment";
			case GL_GEOMETRY_SHADER: return "Geometry";
			default: return "Unknown";
		}
	}
}
