#include "OpenGL/OpenGLWindow.h"
#include "Logging/LoggingMacros.h"
#include "Misc/AssertionMacros.h"
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
		BS_LOG(Debug, "Creating FOpenGLWindow...");
		const int GLFWInitResult = glfwInit();
		BS_ASSERT(GLFWInitResult, "FOpenGLWindow::Init: Failed to init GLFW");
		Window = glfwCreateWindow(WindowSize.x, WindowSize.y, WindowName, nullptr, nullptr);
		BS_ASSERT(Window, "FOpenGLWindow::Init: Failed to create GLFW Window");
		glfwSetWindowUserPointer(Window, &WindowSize);
		glfwSetWindowSizeCallback(Window,
			[](GLFWwindow* const Window, const int Width, const int Height)
		{
			*reinterpret_cast<glm::uvec2*>(glfwGetWindowUserPointer(Window)) = {Width, Height};
			glViewport(0, 0, Width, Height);
		});
		glfwMakeContextCurrent(Window);
		if (!bVSyncEnabled)
		{
			glfwSwapInterval(0);
		}
		const int GladLoadGLResult = gladLoadGL();
		BS_ASSERT(GladLoadGLResult, "FOpenGLWindow::Init: Failed to load GLAD");
		BS_LOG(Info, "OpenGL {}.{} has been loaded", GLVersion.major, GLVersion.minor);
	}

	void FOpenGLWindow::Dispose()
	{
		BS_LOG(Debug, "Destroying FOpenGLWindow...");
		glfwTerminate();
	}

	int FOpenGLWindow::GetKeyState(const int KeyCode)
	{
		return glfwGetKey(Window, KeyCode);
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
}
