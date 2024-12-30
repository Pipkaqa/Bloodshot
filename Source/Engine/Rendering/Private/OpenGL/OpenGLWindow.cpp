#include "OpenGL/OpenGLWindow.h"

#include "AssertionMacros.h"
#include "Casts.h"
#include "Logging/LoggingMacros.h"
#include "OpenGL/OpenGLHeader.h"

namespace Bloodshot
{
	FOpenGLWindow::FOpenGLWindow(const char* WindowName, const glm::uvec2& WindowSize, const bool bVSyncEnabled)
		: IWindow(WindowName, WindowSize, bVSyncEnabled)
	{
		Instance = this;
	}

	void FOpenGLWindow::Init()
	{
		BS_LOG(Debug, "Creating OpenGLWindow...");

		BS_CHECK(glfwInit(), "Failed to init GLFW");

		Window = glfwCreateWindow(WindowSize.x, WindowSize.y, WindowName, nullptr, nullptr);

		BS_CHECK(Window, "Failed to create GLFW Window");

		glfwSetWindowUserPointer(Window, &WindowSize);

		glfwSetWindowSizeCallback(Window,
			[](GLFWwindow* const Window, const int Width, const int Height)
			{
				*ReinterpretCast<glm::uvec2*>(glfwGetWindowUserPointer(Window)) = {Width, Height};
				glViewport(0, 0, Width, Height);
			});

		glfwMakeContextCurrent(Window);

		if (!bVSyncEnabled)
		{
			glfwSwapInterval(0);
		}

		BS_CHECK(gladLoadGL(), "Failed to load GLAD");

		BS_LOG(Info, "OpenGL {0}.{1} has been loaded", GLVersion.major, GLVersion.minor);
	}

	void FOpenGLWindow::Dispose()
	{
		BS_LOG(Debug, "Destroying OpenGLWindow...");

		glfwTerminate();
	}

	bool FOpenGLWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(Window);
	}

	void FOpenGLWindow::SetVSync(const bool bEnabled)
	{
		glfwSwapInterval(bEnabled);

		bVSyncEnabled = bEnabled;
	}

	void FOpenGLWindow::SwapBuffers()
	{
		glfwSwapBuffers(Window);
	}

	void FOpenGLWindow::PollEvents()
	{
		glfwPollEvents();
	}

	void FOpenGLWindow::Close()
	{
		glfwSetWindowShouldClose(Window, GL_TRUE);
	}

	EKeyState FOpenGLWindow::GetKeyState(const EKeyCode KeyCode)
	{
		return StaticCast<EKeyState>(glfwGetKey(Window, StaticCast<int>(KeyCode)));
	}
}
