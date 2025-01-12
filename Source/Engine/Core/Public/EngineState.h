#pragma once

#include "Platform/Platform.h"
#include "Templates/Singleton.h"

namespace Bloodshot
{
	class FEngineState final : public TSingleton<FEngineState>
	{
	public:
		FEngineState();

		bool bRunning = false;
		bool bSimulating = false;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD FORCEINLINE static bool IsRunning()
		{
			return Instance->bRunning;
		}

		NODISCARD FORCEINLINE static bool IsSimulating()
		{
			return Instance->bSimulating;
		}
	};
}
