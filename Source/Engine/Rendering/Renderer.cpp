#include "Renderer.h"

#include "Debug/Logger.h"
#include "Platform/OpenGL/OpenGLRenderer.h"

namespace Bloodshot
{
	Renderer* Renderer::Create(const Config& config)
	{
		FL_CORE_ASSERT(!s_Instance, "An attempt to create another renderer instance");

		if (s_Instance) return s_Instance;

		FL_CORE_DEBUG("Creating renderer...");

		switch (config.m_Type)
		{
			case Type::OpenGL:
			{
				s_Instance = new OpenGLRenderer;
				break;
			}
		}

		s_Instance->m_Config = &config;

		return s_Instance;
	}
}
