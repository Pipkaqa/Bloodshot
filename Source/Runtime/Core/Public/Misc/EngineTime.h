#pragma once

#include "Platform/Platform.h"

#include <chrono>

namespace Bloodshot
{
	class FEngineTime final
	{
		friend class Private::IEngineContext;
		friend class Private::FEngineEditorContext;
		friend class Private::FEngineGameContext;

	public:
		static FEngineTime& GetInstance();

		NODISCARD FORCEINLINE static float GetGlobalTime()
		{
			return GetInstance().GlobalTime;
		}

	private:
		FEngineTime() {}

		const std::chrono::high_resolution_clock::time_point StartTimepoint = std::chrono::high_resolution_clock::now();
		float GlobalTime = 0.f;

		void Tick();
	};
}
