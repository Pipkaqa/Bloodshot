#pragma once

#include "Math/Math.h"
#include "Core/EngineFramework.h"
#include "Utility/ISingleton.h"

namespace Bloodshot
{
	class Window abstract : public ISingleton<Window>
	{
		OWNED_BY_CORE;
		RENDERING_PART;

	public:
		struct Config final
		{
			const char* m_WindowName = "Bloodshot Engine";

			const char* m_IconPath = "Project/Icon";

			const glm::ivec2 m_InitialScreenSize = {600, 600};

			const int m_TargetFPS = 60;

			const int m_ExitKey = NULL;
		};

		virtual ~Window() {}

		NODISCARD FORCEINLINE virtual int GetWidth() const noexcept = 0;
		NODISCARD FORCEINLINE virtual int GetHeight() const noexcept = 0;
		NODISCARD FORCEINLINE virtual int GetFPS() const noexcept = 0;
		NODISCARD FORCEINLINE virtual bool ShouldClose() const noexcept = 0;

		FORCEINLINE virtual void Close() const noexcept = 0;

		NODISCARD FORCEINLINE static const Config& GetConfig() noexcept
		{
			return *s_Instance->m_Config;
		}

	protected:
		Window() {}

		const Config* m_Config = nullptr;

	private:
		using ISingleton::Create;

		NODISCARD static Window* Create(const Config& config);
	};
}
