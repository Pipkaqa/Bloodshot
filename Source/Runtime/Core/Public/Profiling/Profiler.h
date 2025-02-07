#pragma once

#ifdef BS_PROFILING_ON

#include "Containers/Map.h"
#include "Containers/String.h"
#include "Templates/Singleton.h"

#include <chrono>
#include <fstream>

namespace Bloodshot
{
	class FProfiler final : TSingleton<FProfiler>
	{
		friend class FEngineLoop;
		friend class FProfileTimer;

	private:
		FProfiler();

		struct FFunctionProfile final
		{
			FStringView Name;
			bool bMangled;
			size_t TotalExecutions;
			std::chrono::milliseconds TotalExecutionDuration;
			float AverageExecutionDurationMilli;
		};

		using FFunctionProfileMap = TMap<size_t, FFunctionProfile>;

		std::ofstream OutputStream;
		FFunctionProfileMap FunctionProfiles;

		bool bSessionStarted = false;

		NODISCARD static bool IsSessionStarted();

		static void WriteFunctionProfile(FStringView Name, const std::chrono::milliseconds Duration, const bool bMangled);

		void BeginSession();
		void EndSession();
	};
}

#endif
