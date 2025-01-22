#pragma once

#ifdef BS_PROFILING_ON
#include "Templates/Containers/Map.h"
#include "Templates/Containers/String.h"
#include "Templates/Containers/Tuple.h"
#include "Templates/Singleton.h"

#include <chrono>
#include <fstream>

namespace Bloodshot
{
	class FProfiler final : TStaticSingleton<FProfiler>
	{
		friend class FProfileTimer;

	public:
		struct FFunctionProfile final
		{
			FStringView Name;
			bool bMangled;
			size_t TotalExecutions;
			std::chrono::milliseconds TotalExecutionDuration;
			float AverageExecutionDurationMilli;
		};

		using FFunctionProfileMap = TMap<size_t, FFunctionProfile>;

		FProfiler();

		std::ofstream OutputStream;
		FFunctionProfileMap FunctionProfiles;

		bool bSessionStarted = false;

		static bool IsSessionStarted();

		void BeginSession();
		void EndSession();

	private:
		static void WriteFunctionProfile(FStringView Name, const std::chrono::milliseconds Duration, const bool bMangled);
	};
}
#endif
