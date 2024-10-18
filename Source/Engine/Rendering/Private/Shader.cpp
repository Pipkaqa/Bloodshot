#include "Shader.h"

#include "Renderer.h"

namespace Bloodshot
{
	IShader::IShader(std::string_view Name)
		: Name(Name)
	{
	}
}
