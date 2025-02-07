#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "Misc/AssertionMacros.h"

#include <list>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType, typename InAllocatorType = std::allocator<InElementType>>
	using TList = std::list<InElementType, InAllocatorType>;
}