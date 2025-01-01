#include "Window.h"

namespace Bloodshot
{
	IWindow::IWindow(const char* WindowName, const glm::uvec2& WindowSize, const bool bVSyncEnabled)
		: WindowName(WindowName)
		, WindowSize(WindowSize)
		, bVSyncEnabled(bVSyncEnabled)
	{
		Instance = this;
	}

	void IWindow::BeginFrame()
	{
		FrameBeginTimepoint = std::chrono::high_resolution_clock::now();
	}

	void IWindow::EndFrame()
	{
		std::chrono::duration<float> FrameTimeInSec = std::chrono::high_resolution_clock::now() - FrameBeginTimepoint;
		FrameTimeInMilli = FrameTimeInSec.count() * 1000.f;
	}
}
