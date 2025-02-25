#pragma once

#include "Core.h"

namespace Bloodshot
{
	class IWindow
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;
		friend class IInput;

	public:
		virtual ~IWindow() {}

		NODISCARD static IWindow* GetInstance();

		NODISCARD static uint32_t GetWidth() noexcept;
		NODISCARD static uint32_t GetHeight() noexcept;
		NODISCARD static float GetFrameTime() noexcept;

	protected:
		IWindow(const char* WindowName = "Bloodshot Game Engine",
			const glm::uvec2& WindowSize = {640, 480},
			const bool bVSyncEnabled = false);

		static inline IWindow* Instance = nullptr;

		const char* WindowName;
		glm::uvec2 WindowSize;
		bool bVSyncEnabled;

		virtual void Init() = 0;
		virtual void Dispose() = 0;

	private:
		std::chrono::high_resolution_clock::time_point FrameBeginTimepoint;
		float FrameTimeInMilli = 0.f;

		NODISCARD virtual int GetKeyState(const int KeyCode) = 0;
		NODISCARD virtual bool ShouldClose() const = 0;
		virtual void SetVSync(const bool bEnabled) = 0;
		virtual void SwapBuffers() = 0;
		virtual void PollEvents() = 0;
		virtual void Close() = 0;

		void BeginFrame();
		void EndFrame();
	};
}
