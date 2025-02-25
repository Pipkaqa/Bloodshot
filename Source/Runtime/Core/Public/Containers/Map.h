#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "Misc/AssertionMacros.h"

#include "Containers/Pair.h"

#include <map>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InKeyType,
		typename InValueType,
		typename InPredicateType = std::less<InKeyType>,
		typename InAllocatorType = std::allocator<TPair<const InKeyType, InValueType>>>
	using TMap = std::map<InKeyType, InValueType, InPredicateType, InAllocatorType>;
}
