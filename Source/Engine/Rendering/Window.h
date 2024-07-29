#pragma once

#include "Core/EngineTime.h"
#include "Math/Math.h"

namespace Bloodshot
{
	class Window abstract : public ISingleton<Window>
	{
		CORE_MODULE;
		RENDERER_MODULE;

	public:
		struct Config final
		{
			const char* m_WindowName = "Bloodshot Game Engine";

			//const char* m_IconPath = "Project/Icon";

			glm::uvec2 m_ScreenSize = {640, 480};

			//const int m_TargetFPS = 60;

			bool m_VSync = false;

			//const int m_ExitKey = NULL;
		};

		virtual ~Window() {};

		NODISCARD virtual uint32_t GetWidth() const noexcept = 0;
		NODISCARD virtual uint32_t GetHeight() const noexcept = 0;

		NODISCARD virtual bool ShouldClose() const noexcept = 0;

		virtual void SetVSync(bool vsync) = 0;

		virtual void Close() noexcept = 0;

		NODISCARD FORCEINLINE static const Config& GetConfig() noexcept
		{
			return s_Instance->m_Config;
		}

		NODISCARD FORCEINLINE static size_t GetFramesPerSecond() noexcept
		{
			return s_Instance->m_LastFramesPerSecondValue;
		}

		NODISCARD FORCEINLINE static float GetFrameTime() noexcept
		{
			return s_Instance->m_FrameTime;
		}

	protected:
		Config m_Config = {};
		std::chrono::high_resolution_clock::time_point m_FrameBegin;
		size_t m_LastFramesPerSecondValue = 0;
		size_t m_FrameCount = 0;
		float m_FrameTimeAccumulation = 0;
		float m_FrameTime = 0;

	private:
		using ISingleton::Create;

		Window() {}

		NODISCARD static Window* Create(const Config& config);

		virtual void SwapBuffers() const = 0;
		virtual void PollEvents() const = 0;

		void BeginFrame();
		void EndFrame();
	};
}
