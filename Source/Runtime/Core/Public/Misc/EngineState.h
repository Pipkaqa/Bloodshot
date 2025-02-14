#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	class FEngineState final
	{
		friend class Private::IEngineContext;
		friend class Private::FEngineEditorContext;
		friend class Private::FEngineGameContext;

	public:
		static FEngineState& GetInstance();

		NODISCARD FORCEINLINE static bool IsRunning()
		{
			return GetInstance().bRunning;
		}

		NODISCARD FORCEINLINE static bool IsSimulating()
		{
			return GetInstance().bSimulating;
		}

	private:
		FEngineState() {}

		bool bRunning = false;
		bool bSimulating = false;
	};
}
