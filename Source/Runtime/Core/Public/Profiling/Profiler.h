#pragma once

#ifdef BS_PROFILING_ON

#include "Containers/Map.h"
#include "Containers/String.h"
#include "Templates/Singleton.h"

#include <chrono>
#include <fstream>

namespace Bloodshot
{
	class FProfiler final
	{
		friend class FEngineLoop;
		friend class FProfileTimer;

	private:
		struct FFunctionProfile final
		{
			FStringView Name;
			bool bMangled;
			size_t TotalExecutions;
			std::chrono::milliseconds TotalExecutionDuration;
			float AverageExecutionDurationMilli;
		};

		using FFunctionProfileMap = TMap<size_t, FFunctionProfile>;

		FProfiler() = default;

		std::ofstream OutputStream;
		FFunctionProfileMap FunctionProfiles;

		bool bSessionStarted = false;

		NODISCARD static FProfiler& GetInstance();

		NODISCARD FORCEINLINE bool IsSessionStarted()
		{
			return bSessionStarted;
		}

		void BeginSession();
		void EndSession();

		void WriteFunctionProfile(FStringView Name, const std::chrono::milliseconds Duration, const bool bMangled);
	};
}

#endif
