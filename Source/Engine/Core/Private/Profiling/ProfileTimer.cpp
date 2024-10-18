#ifdef BS_PROFILING_ON
#include "Profiling/ProfileTimer.h"
#include "Profiling/Profiler.h"

#include <chrono>

namespace Bloodshot
{
	FProfileTimer::FProfileTimer(const char* RangeName, const bool bFunctionSignaturePassed)
		: RangeName(RangeName)
		, bFunctionSignaturePassed(bFunctionSignaturePassed)
	{
	}

	FProfileTimer::~FProfileTimer()
	{
		const std::chrono::high_resolution_clock::time_point EndTimepoint = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<float> RangeDuration = EndTimepoint - StartTimepoint;

		FProfiler::WriteRangeProfile(RangeName, RangeDuration.count() * 1000.f, bFunctionSignaturePassed);
	}
}
#endif
