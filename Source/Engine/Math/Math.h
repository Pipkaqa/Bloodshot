#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/Platform.h"

#include <random>

namespace Bloodshot
{
	class Random abstract final
	{
	public:
		NODISCARD FORCEINLINE static uint64_t Get()
		{
			return s_UniformDistribution(s_Engine);
		}

		NODISCARD FORCEINLINE static uint64_t Get(uint64_t min, uint64_t max)
		{
			return std::uniform_int_distribution<uint64_t>(min, max)(s_Engine);
		}

	private:
		static inline std::random_device s_RandomDevice;
		static inline std::mt19937_64 s_Engine{Random::s_RandomDevice()};
		static inline std::uniform_int_distribution<uint64_t> s_UniformDistribution;
	};
}
