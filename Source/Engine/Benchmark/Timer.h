#pragma once

#include "Utility/Utility.h"

#include <chrono>

namespace Bloodshot
{
	class Timer final
	{
	public:
		Timer();
		~Timer() {}

		NODISCARD std::chrono::microseconds GetElapsedMilliseconds() const;
		NODISCARD std::chrono::microseconds GetElapsedMicroseconds() const;

		FORCEINLINE void Reset()
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	};
}
