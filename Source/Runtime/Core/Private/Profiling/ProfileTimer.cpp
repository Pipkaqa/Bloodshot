#ifdef BS_PROFILING_ON

#include "Profiling/ProfileTimer.h"
#include "Profiling/Profiler.h"

#include <chrono>

namespace Bloodshot
{
	FProfileTimer::FProfileTimer(const char* FunctionName, const bool bMangled)
		: FunctionName(FunctionName)
		, bMangled(bMangled)
	{
	}

	FProfileTimer::~FProfileTimer()
	{
		std::chrono::milliseconds DurationInMilli = Super::GetElapsedMilliseconds();
		FProfiler::GetInstance().WriteFunctionProfile(FunctionName, DurationInMilli, bMangled);
	}
}

#endif
