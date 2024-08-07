#pragma once

#include "Platform.h"

#include <chrono>

namespace Bloodshot
{
	class STimer
	{
	public:
		NODISCARD std::chrono::microseconds GetElapsedMilliseconds() const noexcept;
		NODISCARD std::chrono::microseconds GetElapsedMicroseconds() const noexcept;

		FORCEINLINE void Reset() noexcept
		{
			StartTimepoint = std::chrono::high_resolution_clock::now();
		}

	protected:
		std::chrono::time_point<std::chrono::high_resolution_clock> StartTimepoint = std::chrono::high_resolution_clock::now();
	};
}
