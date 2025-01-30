#pragma once

#include <concepts>
#include <type_traits>

namespace Bloodshot
{
	template<typename T>
	concept IsIntegral = std::is_integral_v<T>;

	template<typename T>
	concept IsFloatingPoint = std::is_floating_point_v<T>;

	template <typename T>
	concept IsContainer = requires(T Container)
	{
		typename T::iterator;
		{ Container.begin() } -> std::same_as<typename T::iterator>;
		{ Container.end() } -> std::same_as<typename T::iterator>;
	};

	template<typename T>
	struct TFalseType : std::false_type {};
}
