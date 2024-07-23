#include "OpenGLWindow.h"

#include "Platform/OpenGL/OpenGLHeader.h"

#include "Debug/Logger.h"

namespace Bloodshot
{
	void OpenGLWindow::Init()
	{
		FL_CORE_DEBUG("Creating OpenGL window...");

		//TODO: wrap to glfw check func
		if (!glfwInit())
		{
			FL_CORE_FATAL("GLFW init failed!");
		}

		//TODO: wrap to glfw check func
		m_Window = glfwCreateWindow(m_Config->m_InitialScreenSize.x, m_Config->m_InitialScreenSize.y, m_Config->m_WindowName, nullptr, nullptr);

		if (!m_Window)
		{
			FL_CORE_FATAL("Failed to create GLFW window!");
			glfwTerminate();
		}

		//TODO: wrap to glfw check func
		glfwMakeContextCurrent(m_Window);

		//TODO: wrap to glfw check func
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			FL_CORE_FATAL("Failed to initialize GLAD!");
		}
	}

	void OpenGLWindow::Dispose()
	{
		FL_CORE_DEBUG("Destroying OpenGL window...");

		glfwTerminate();
	}

	int OpenGLWindow::GetWidth() const noexcept
	{
		int width;

		glfwGetWindowSize(m_Window, &width, nullptr);

		return width;
	}

	int OpenGLWindow::GetHeight() const noexcept
	{
		int height;

		glfwGetWindowSize(m_Window, nullptr, &height);

		return height;
	}

	int OpenGLWindow::GetFPS() const noexcept
	{
		//TODO: do fps;
		//TODO: maybe put fps in Core?
		return 0;
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
