#pragma once

#include "Allocators/Allocator.h"
#include "Templates/IsContiguousContainer.h"

namespace Bloodshot
{
	template<typename CharType, template<typename> typename AllocatorType = TAllocator>
	class TString;

	template<typename CharType, template<typename> typename AllocatorType>
	struct TIsContiguousContainer<TString<CharType, AllocatorType>>
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
