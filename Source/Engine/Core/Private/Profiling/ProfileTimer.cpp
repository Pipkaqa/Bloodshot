#include "Profiling/ProfileTimer.h"

#include "Profiling/Profiler.h"

#include <chrono>

namespace Bloodshot
{
	SProfileTimer::SProfileTimer(const char* RangeName, const bool bFunctionSignaturePassed)
		: RangeName(RangeName), bFunctionSignaturePassed(bFunctionSignaturePassed)
	{
	}

	SProfileTimer::~SProfileTimer()
	{
		const std::chrono::high_resolution_clock::time_point EndTimepoint = std::chrono::high_resolution_clock::now();

		const std::chrono::duration<float> RangeDuration = EndTimepoint - StartTimepoint;

		SProfiler::WriteRangeProfile(RangeName, RangeDuration.count() * 1000.f, bFunctionSignaturePassed);
	}
}
