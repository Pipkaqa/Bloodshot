#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "AssertionMacros.h"

#include <unordered_map>
#include <map>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename KeyType,
		typename ValueType,
		typename HasherType = std::hash<KeyType>,
		typename KeyEqualType = std::equal_to<KeyType>,
		typename AllocatorType = std::allocator<std::pair<const KeyType, ValueType>>>
	using TUnorderedMap = std::unordered_map<KeyType, ValueType, HasherType, KeyEqualType, AllocatorType>;

	template<typename KeyType, 
		typename ValueType, 
		typename PredicateType = std::less<KeyType>, 
		typename AllocatorType = std::allocator<std::pair<const KeyType, ValueType>>>
	using TMap = std::map<KeyType, ValueType, PredicateType, AllocatorType>;
}