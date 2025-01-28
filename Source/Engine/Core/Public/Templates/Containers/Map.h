#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "AssertionMacros.h"

#include "Templates/Containers/Pair.h"

#include <map>
#include <unordered_map>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InKeyType,
		typename InValueType,
		typename InPredicateType = std::less<InKeyType>,
		typename InAllocatorType = std::allocator<TPair<const InKeyType, InValueType>>>
	using TMap = std::map<InKeyType, InValueType, InPredicateType, InAllocatorType>;

	template<typename InKeyType,
		typename InValueType,
		typename InHasherType = std::hash<InKeyType>,
		typename InKeyEqualType = std::equal_to<InKeyType>,
		typename InAllocatorType = std::allocator<TPair<const InKeyType, InValueType>>>
	using TUnorderedMap = std::unordered_map<InKeyType, InValueType, InHasherType, InKeyEqualType, InAllocatorType>;
}