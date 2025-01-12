#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	template<typename T, typename ArgType>
	NODISCARD FORCEINLINE constexpr T StaticCast(const ArgType& Src)
	{
		return static_cast<T>(Src);
	}

	template<typename T, typename ArgType>
	NODISCARD FORCEINLINE constexpr T ReinterpretCast(const ArgType& Src)
	{
		return reinterpret_cast<T>(Src);
	}
}
