#include "Timer.h"

namespace Bloodshot
{
	using namespace std::chrono;

	template<typename In>
	FORCEINLINE static In GetTimeSinceEpochIn(const high_resolution_clock::time_point& TimePoint)
	{
		return time_point_cast<In>(TimePoint).time_since_epoch();
	}

	microseconds STimer::GetElapsedMilliseconds() const noexcept
	{
		const high_resolution_clock::time_point EndTimepoint = high_resolution_clock::now();

		const milliseconds StartTime = GetTimeSinceEpochIn<milliseconds>(StartTimepoint);
		const milliseconds EndTime = GetTimeSinceEpochIn<milliseconds>(EndTimepoint);

		return EndTime - StartTime;
	}

	microseconds STimer::GetElapsedMicroseconds() const noexcept
	{
		const high_resolution_clock::time_point EndTimepoint = high_resolution_clock::now();

		const microseconds StartTime = GetTimeSinceEpochIn<microseconds>(StartTimepoint);
		const microseconds EndTime = GetTimeSinceEpochIn<microseconds>(EndTimepoint);

		return EndTime - StartTime;
	}
}
