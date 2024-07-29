#pragma once

#include "Platform/Platform.h"

#include <chrono>

namespace Bloodshot
{
	class Timer
	{
	public:
		Timer();

		NODISCARD std::chrono::microseconds GetElapsedMilliseconds() const;
		NODISCARD std::chrono::microseconds GetElapsedMicroseconds() const;

		FORCEINLINE void Reset() noexcept
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

	protected:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	};
}
