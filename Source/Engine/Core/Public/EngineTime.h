#pragma once

#include "Platform/Platform.h"
#include "Templates/Singleton.h"

#include <chrono>

namespace Bloodshot
{
	class FEngineTime final : TSingleton<FEngineTime>
	{
	public:
		FEngineTime();

		const std::chrono::high_resolution_clock::time_point StartTimepoint = std::chrono::high_resolution_clock::now();

		float GlobalTime = 0.f;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD FORCEINLINE static float GetGlobalTime()
		{
			return Instance->GlobalTime;
		}

		void Tick();
	};
}
