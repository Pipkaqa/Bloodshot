#include "Window.h"

namespace Bloodshot
{
	IWindow::IWindow(const char* WindowName, const glm::uvec2& WindowSize, const bool bVSyncEnabled)
		: WindowName(WindowName), WindowSize(WindowSize), bVSyncEnabled(bVSyncEnabled)
	{
		Instance = this;
	}

	void IWindow::BeginFrame()
	{
		FrameBeginTimepoint = std::chrono::high_resolution_clock::now();
	}

	void IWindow::EndFrame()
	{
		std::chrono::high_resolution_clock::time_point FrameEndTimepoint = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> FrameTimeInSec = FrameEndTimepoint - FrameBeginTimepoint;

		++FrameCount;
		FrameTimeInMilli = FrameTimeInSec.count() * 1000.f;
		FrameTimeInMilliAccumulation += FrameTimeInMilli;

		if (FrameTimeInMilliAccumulation >= 1000.f)
		{
			FrameCountInPrevSec = FrameCount;
			FrameTimeInMilliAccumulation = 0.f;
			FrameCount = 0U;
		}
	}
}
