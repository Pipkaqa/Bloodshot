#pragma once

#include <cstdint>
#include <limits>

namespace Bloodshot
{
	struct IReservedValues final
	{
		static constexpr size_t NoneIndex = std::numeric_limits<size_t>::max();
		static constexpr uint32_t NoneTypeID = std::numeric_limits<uint32_t>::max();
	};
}
