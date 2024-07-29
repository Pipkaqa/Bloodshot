#define GLAD_GL_IMPLEMENTATION
#include "OpenGLHeader.h"

namespace Bloodshot
{
	const char* OpenGLShaderTypeToString(GLenum type)
	{
		switch (type)
		{
			case GL_VERTEX_SHADER:
			{
				return "Vertex";
			}
			case GL_FRAGMENT_SHADER:
			{
				return "Fragment";
			}
			case GL_GEOMETRY_SHADER:
			{
				return "Geometry";
			}
		}

		return "Unknown";
	}
}