#include "Window.h"

#include "Debug/Logger.h"
#include "Platform/OpenGL/OpenGLWindow.h"
#include "Renderer.h"

namespace Bloodshot
{
	Window* Window::Create(const Config& config)
	{
		BS_ASSERT(!s_Instance, "An attempt to create another window instance");

		switch (Renderer::GetType())
		{
			case Renderer::Type::OpenGL:
			{
				s_Instance = new OpenGLWindow;
				break;
			}

			// BSTODO: Add Vulkan support
		}

		BS_CHECK_FATAL(s_Instance, "Failed to create window!");

		s_Instance->m_Config = config;

		return s_Instance;
	}

	void Window::BeginFrame()
	{
		m_FrameBegin = std::chrono::high_resolution_clock::now();
	}

	void Window::EndFrame()
	{
		auto frameEnd = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> frameDuration = frameEnd - m_FrameBegin;

		m_FrameCount++;
		m_FrameTime = frameDuration.count() * 1000.f;
		m_FrameTimeAccumulation += m_FrameTime;

		if (m_FrameTimeAccumulation >= 1000.f)
		{
			m_LastFramesPerSecondValue = m_FrameCount;
			m_FrameTimeAccumulation = 0.f;
			m_FrameCount = 0;
		}
	}
}
