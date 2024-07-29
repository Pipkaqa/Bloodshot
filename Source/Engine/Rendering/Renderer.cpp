#include "Renderer.h"

#include "Debug/Logger.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Bloodshot
{
	Renderer* Renderer::Create(const Config& config)
	{
		BS_ASSERT(!s_Instance, "An attempt to create another renderer");

		switch (config.m_Type)
		{
			case Type::OpenGL:
			{
				s_Instance = new OpenGLRenderer;
				break;
			}

			// BSTODO: Add Vulkan support
		}

		BS_CHECK_FATAL(s_Instance, "Failed to create renderer!");

		s_Instance->m_Config = config;

		return s_Instance;
	}
}
