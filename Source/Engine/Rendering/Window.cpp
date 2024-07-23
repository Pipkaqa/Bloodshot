#include "Window.h"

#include "Renderer.h"
#include "Debug/Logger.h"
#include "Platform/OpenGL/OpenGLWindow.h"

namespace Bloodshot
{
	Window* Window::Create(const Config& config)
	{
		FL_CORE_ASSERT(!s_Instance, "An attempt to create another window instance");

		if (s_Instance) return s_Instance;

		switch (Renderer::GetType())
		{
			case Renderer::Type::OpenGL:
			{
				s_Instance = new OpenGLWindow;
				break;
			}
		}

		s_Instance->m_Config = &config;

		return s_Instance;
	}
}
