#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform.h"

#include <random>

namespace Bloodshot
{
	template<typename T>
	class Random abstract final
	{
	public:
		NODISCARD FORCEINLINE static T Get()
		{
			return s_UniformDistribution(s_Engine);
		}

		NODISCARD FORCEINLINE static T Get(const T Min, const T Max)
		{
			return std::uniform_int_distribution<T>(Min, Max)(s_Engine);
		}

	private:
		static inline std::random_device s_RandomDevice;
		static inline std::mt19937_64 s_Engine{s_RandomDevice()};
		static inline std::uniform_int_distribution<T> s_UniformDistribution;
	};

	class FTransform
	{
	public:
		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Rotation = glm::vec3(0.f);
		glm::vec3 Scale = glm::vec3(1.f);
	};

	struct FVector3 final
	{
		static constexpr inline glm::vec3 Up = glm::vec3(0.f, 1.f, 0.f);
		static constexpr inline glm::vec3 Forward = glm::vec3(0.f, 0.f, 1.f);
		static constexpr inline glm::vec3 Right = glm::vec3(1.f, 0.f, 0.f);
	};
}
