#pragma once

#include "OpenGL/OpenGLHeader.h"

#include <string>

namespace Bloodshot::Launcher
{
	struct FTexture final
	{
		GLuint UniqueID = 0;
		std::string Name = {};

		int Width = 0;
		int Height = 0;
		int Channels = 0;
	};
}
