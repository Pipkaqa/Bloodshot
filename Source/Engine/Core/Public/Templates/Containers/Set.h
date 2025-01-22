#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "AssertionMacros.h"

#include <set>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType, 
		typename Predicate = std::less<InElementType>, 
		typename InAllocatorType = std::allocator<InElementType>>
	using TSet = std::set<InElementType, Predicate, InAllocatorType>;
}