#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "Misc/AssertionMacros.h"

#include "Containers/Pair.h"

#include <unordered_map>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InKeyType,
		typename InValueType,
		typename InHasherType = std::hash<InKeyType>,
		typename InKeyEqualType = std::equal_to<InKeyType>,
		typename InAllocatorType = std::allocator<TPair<const InKeyType, InValueType>>>
	using TUnorderedMap = std::unordered_map<InKeyType, InValueType, InHasherType, InKeyEqualType, InAllocatorType>;
}
