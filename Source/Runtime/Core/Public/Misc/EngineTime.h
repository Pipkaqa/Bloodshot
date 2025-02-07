#pragma once

#include "Platform/Platform.h"
#include "Templates/Singleton.h"

#include <chrono>

namespace Bloodshot
{
	class FEngineTime final : TManager<FEngineTime>
	{
		friend class IEngineContext;
		friend class FEngineEditorContext;
		friend class FEngineGameContext;

	public:
		NODISCARD static float GetGlobalTime();

	private:
		FEngineTime();

		const std::chrono::high_resolution_clock::time_point StartTimepoint = std::chrono::high_resolution_clock::now();

		float GlobalTime = 0.f;

		virtual void Init() override;
		virtual void Dispose() override;

		void Tick();
	};
}
