#pragma once

#include "Allocators/Allocator.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	namespace Private::AllocatorTraits
	{
		template<IsAllocator AllocatorType, typename ElementType>
		class TGetAllocatorForElementTypeSpecializationIfPossible
		{
		private:
			template<typename U>
			static auto Test(int) -> decltype(typename U::template ForElementType<ElementType>{});
			template<typename U>
			static U::ForAnyElementType Test(...);
		public:
			using Type = decltype(Test<AllocatorType>(0));
		};

		template<IsAllocator AllocatorType, typename ElementType>
		using TGetAllocatorForElementTypeSpecializationIfPossible_T = TGetAllocatorForElementTypeSpecializationIfPossible<AllocatorType, ElementType>::Type;
	}

	template<typename AllocatorType>
	class TAllocatorTraits;

	template<IsNonSpecializedAllocator AllocatorType>
	class TAllocatorTraits<AllocatorType>
	{
	public:
		using ForAnyElementType = typename AllocatorType::ForAnyElementType;

		template<typename ElementType>
		using ForElementType = Private::AllocatorTraits::TGetAllocatorForElementTypeSpecializationIfPossible_T<AllocatorType, ElementType>;

		class View
		{
		public:
			using Generic =
				std::conditional_t<IsContiguousAllocator<ForAnyElementType>,
				FAllocatorView::Contiguous::Generic,
				FAllocatorView::NonContiguous::Generic>;

			using Typed =
				std::conditional_t<IsContiguousAllocator<ForElementType<void*>>,
				FAllocatorView::Contiguous::Typed,
				FAllocatorView::NonContiguous::Generic>;
		};

		static constexpr bool IsContiguous = IsContiguousAllocator<ForElementType<void*>>;
	};

	template<IsSpecializedAllocator AllocatorType>
	class TAllocatorTraits<AllocatorType>
	{
	public:
		using ForAnyElementType = AllocatorType;

		template<typename ElementType>
		using ForElementType = AllocatorType;

		class View
		{
		public:
			using Generic =
				std::conditional_t<IsContiguousAllocator<AllocatorType>,
				FAllocatorView::Contiguous::Generic,
				FAllocatorView::NonContiguous::Generic>;

			using Typed =
				std::conditional_t<IsContiguousAllocator<AllocatorType>,
				FAllocatorView::Contiguous::Typed,
				FAllocatorView::NonContiguous::Generic>;
		};

		static constexpr bool IsContiguous = IsContiguousAllocator<AllocatorType>;
	};
}
