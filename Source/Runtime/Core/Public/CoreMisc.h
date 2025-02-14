#pragma once

#include <limits>

namespace Bloodshot
{
	struct IReservedValues final
	{
		static inline constexpr size_t NoneIndex = std::numeric_limits<size_t>::max();
	};
}
