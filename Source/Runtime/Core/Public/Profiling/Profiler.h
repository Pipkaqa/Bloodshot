#pragma once

#ifdef BS_PROFILING_ON

#include "Containers/Map.h"
#include "Platform/Platform.h"

#include <chrono>
#include <fstream>

namespace Bloodshot
{
	class FProfiler final
	{
		friend struct Private::Launch::FDebugEntry;
		friend class FProfileTimer;

		struct FFunctionProfile final
		{
			const char* Name;
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

		NODISCARD FORCEINLINE bool IsSessionStarted() const noexcept
		{
			return bSessionStarted;
		}

		void BeginSession();
		void EndSession();

		void WriteFunctionProfile(const char* Name, const std::chrono::milliseconds Duration, const bool bMangled);
	};
}

#endif
