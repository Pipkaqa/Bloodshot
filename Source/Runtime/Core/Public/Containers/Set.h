#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "Misc/AssertionMacros.h"

#include <set>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType,
		typename InPredicateType = std::less<InElementType>,
		typename InAllocatorType = std::allocator<InElementType>>
		using TSet = std::set<InElementType, InPredicateType, InAllocatorType>;
}