#pragma once

#include "EngineTime.h"
#include "KeyCode.h"
#include "KeyState.h"
#include "MathLibrary.h"

// BSTODO: Finish class

namespace Bloodshot
{
	class IWindow abstract : public TSingleton<IWindow>
	{
	public:
		IWindow(const char* WindowName = "Bloodshot Game Engine",
			const glm::uvec2& WindowSize = {640, 480},
			const bool bVSyncEnabled = false);

		virtual ~IWindow() {}

		const char* WindowName;

		//const char* m_IconPath = "Project/Icon";

		glm::uvec2 WindowSize;

		//const int m_TargetFPS = 60;

		bool bVSyncEnabled;

		//const int m_ExitKey = NULL;

		NODISCARD virtual bool ShouldClose() const = 0;

		NODISCARD virtual EKeyState GetKeyState(const EKeyCode KeyCode) = 0;

		virtual void SetVSync(const bool bEnabled) = 0;

		virtual void SwapBuffers() = 0;
		virtual void PollEvents() = 0;
		virtual void Close() = 0;
		
		NODISCARD FORCEINLINE static uint32_t GetWidth() noexcept
		{
			return Instance->WindowSize.x;
		}

		NODISCARD FORCEINLINE static uint32_t GetHeight() noexcept
		{
			return Instance->WindowSize.x;
		}

		NODISCARD FORCEINLINE static float GetFrameTime() noexcept
		{
			return Instance->FrameTimeInMilli;
		}

		void BeginFrame();
		void EndFrame();

	private:
		std::chrono::high_resolution_clock::time_point FrameBeginTimepoint;
		float FrameTimeInMilli = 0.f;
	};
}
