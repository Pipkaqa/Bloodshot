#include "Timer.h"

namespace Bloodshot
{
	Timer::Timer()
		: m_StartTimepoint(std::chrono::high_resolution_clock::now())
	{
	}

	std::chrono::microseconds Timer::GetElapsedMilliseconds() const
	{
		const auto endTimepoint = std::chrono::high_resolution_clock::now();

		const auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_StartTimepoint).time_since_epoch();
		const auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimepoint).time_since_epoch();

		return end - start;
	}

	std::chrono::microseconds Timer::GetElapsedMicroseconds() const
	{
		const auto endTimepoint = std::chrono::high_resolution_clock::now();

		const auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint);
		const auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint);

		return end - start;
	}
}