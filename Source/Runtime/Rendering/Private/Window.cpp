#include "Window.h"

namespace Bloodshot
{
	IWindow::IWindow(const char* WindowName, const glm::uvec2& WindowSize, const bool bVSyncEnabled)
		: WindowName(WindowName)
		, WindowSize(WindowSize)
		, bVSyncEnabled(bVSyncEnabled)
	{
	}

	uint32_t IWindow::GetWidth() noexcept
	{
		return Instance->WindowSize.x;
	}

	uint32_t IWindow::GetHeight() noexcept
	{
		return Instance->WindowSize.x;
	}

	float IWindow::GetFrameTime() noexcept
	{
		return Instance->FrameTimeInMilli;
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
