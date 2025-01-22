#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "AssertionMacros.h"

#include <string>
#include <xmemory>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InAllocatorType = std::allocator<char>>
	using TString = std::basic_string<char, std::char_traits<char>, InAllocatorType>;

	using FString = TString<std::allocator<char>>;

	using FStringView = std::string_view;
}