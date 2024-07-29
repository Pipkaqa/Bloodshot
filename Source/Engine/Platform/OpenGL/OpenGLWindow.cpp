#include "OpenGLWindow.h"

#include "Debug/Logger.h"
#include "Platform/OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	void OpenGLWindow::Init()
	{
		BS_DEBUG("Creating OpenGL window...");

		BS_CHECK_FATAL(glfwInit(), "Failed to init glfw!");

		m_Window = glfwCreateWindow(m_Config.m_ScreenSize.x, m_Config.m_ScreenSize.y, m_Config.m_WindowName, nullptr, nullptr);

		BS_CHECK_FATAL_WITH_CALL(m_Window, "Failed to create glfw window!", glfwTerminate());

		glfwSetWindowUserPointer(m_Window, &m_Config);

		glfwSetWindowSizeCallback(m_Window,
			[](GLFWwindow* window, int width, int height)
			{
				Config& config = *FastCast<Config*>(glfwGetWindowUserPointer(window));
				config.m_ScreenSize = {width, height};
				glViewport(0, 0, width, height);
			});

		glfwMakeContextCurrent(m_Window);

		if (!m_Config.m_VSync)
		{
			glfwSwapInterval(0);
		}

		BS_CHECK_FATAL(gladLoadGL(), "Failed to load glad!");

		BS_INFO("OpenGL {0}.{1} has been loaded!", GLVersion.major, GLVersion.minor);
	}

	void OpenGLWindow::Dispose()
	{
		BS_DEBUG("Destroying OpenGL window...");

		glfwTerminate();
	}

	uint32_t OpenGLWindow::GetWidth() const noexcept
	{
		return m_Config.m_ScreenSize.x;
	}

	uint32_t OpenGLWindow::GetHeight() const noexcept
	{
		return m_Config.m_ScreenSize.y;
	}

	void OpenGLWindow::SetVSync(bool vsync)
	{
		glfwSwapInterval(vsync);

		m_Config.m_VSync = vsync;
	}

	bool OpenGLWindow::ShouldClose() const noexcept
	{
		return glfwWindowShouldClose(m_Window);
	}

	void OpenGLWindow::SwapBuffers() const
	{
		glfwSwapBuffers(m_Window);
	}

	void OpenGLWindow::PollEvents() const
	{
		glfwPollEvents();
	}

	void OpenGLWindow::Close() noexcept
	{
		glfwSetWindowShouldClose(m_Window, GL_TRUE);
	}
}
