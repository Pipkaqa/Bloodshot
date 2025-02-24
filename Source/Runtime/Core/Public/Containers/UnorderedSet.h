#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "Misc/AssertionMacros.h"

#include <unordered_set>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType,
		typename InHasherType = std::hash<InElementType>,
		typename InKeyEqualType = std::equal_to<InElementType>,
		typename InAllocatorType = std::allocator<InElementType>>
	using TUnorderedSet = std::unordered_set<InElementType, InHasherType, InKeyEqualType, InAllocatorType>;
}