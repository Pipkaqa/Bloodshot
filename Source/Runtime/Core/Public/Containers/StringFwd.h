#pragma once

#include "Allocators/Allocator.h"
#include "Templates/IsContiguousContainer.h"

namespace Bloodshot
{
	template<typename CharType, IsAllocator AllocatorType = FDefaultAllocator>
	class TString;

	template<typename CharType>
	struct TIsContiguousContainer<TString<CharType>>
	{
		static constexpr bool Value = true;
	};

	using FString = TString<char>;

	template <typename CharType>
	class TStringView;

	template <typename CharType>
	struct TIsContiguousContainer<TStringView<CharType>>
	{
		static constexpr bool Value = true;
	};

	using FStringView = TStringView<char>;
}
