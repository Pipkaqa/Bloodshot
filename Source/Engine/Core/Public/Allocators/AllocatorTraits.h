#pragma once

#include "Allocators/Allocator.h"

namespace Bloodshot
{
	namespace Private
	{
		template <typename T>
		struct HasConstruct final
		{
			using TrueType = char;
			using FalseType = int;

			template <typename U>
			static TrueType HasConstructImpl(decltype(&U::Construct));

			template <typename U>
			static FalseType HasConstructImpl(...);

			static constexpr bool Value = sizeof(HasConstructImpl<T>(0)) == sizeof(TrueType);
		};

		template <typename T>
		struct HasDestroy final
		{
			using TrueType = char;
			using FalseType = int;

			template <typename U>
			static TrueType HasDestroyImpl(decltype(&U::Destroy));

			template <typename U>
			static FalseType HasDestroyImpl(...);

			static constexpr bool Value = sizeof(HasDestroyImpl<T>(0)) == sizeof(TrueType);
		};

		template <typename T>
		struct HasConstructQuantity final
		{
			using TrueType = char;
			using FalseType = int;

			template <typename U>
			static TrueType HasConstructQuantityImpl(decltype(&U::ConstructQuantity));

			template <typename U>
			static FalseType HasConstructQuantityImpl(...);

			static constexpr bool Value = sizeof(HasConstructQuantityImpl<T>(0)) == sizeof(TrueType);
		};

		template <typename T>
		struct HasDestroyQuantity final
		{
			using TrueType = char;
			using FalseType = int;

			template <typename U>
			static TrueType HasDestroyQuantityImpl(decltype(&U::DestroyQuantity));

			template <typename U>
			static FalseType HasDestroyQuantityImpl(...);

			static constexpr bool Value = sizeof(HasDestroyQuantityImpl<T>(0)) == sizeof(TrueType);
		};

		template <typename T>
		struct HasConstructQuantityFrom final
		{
			using TrueType = char;
			using FalseType = int;

			template <typename U>
			static TrueType HasConstructQuantityFromImpl(decltype(&U::ConstructQuantityFrom));

			template <typename U>
			static FalseType HasConstructQuantityFromImpl(...);

			static constexpr bool Value = sizeof(HasConstructQuantityFromImpl<T>(0)) == sizeof(TrueType);
		};
	}

	template<IsAllocator InAllocatorType>
	class IAllocatorTraits
	{
	public:
		using AllocatorType = InAllocatorType;
		using ElementType = AllocatorType::ElementType;
		using PointerType = AllocatorType::PointerType;

		static void* Allocate(AllocatorType& Allocator, const size_t Count)
		{
			return Allocator.Allocate(Count);
		}

		static void Deallocate(AllocatorType& Allocator, void* const Block, const size_t Size)
		{
			Allocator.Deallocate(Block, Size);
		}

		template<typename... ArgTypes>
		static void Construct(AllocatorType& Allocator, PointerType const Element, ArgTypes&&... Args)
		{
			if constexpr (Private::HasConstruct<AllocatorType>::Value)
			{
				Allocator.Construct(Element, std::forward<ArgTypes>(Args)...);
			}
			else
			{
				new(Element) ElementType(std::forward<ArgTypes>(Args)...);
			}
		}

		static void Destroy(AllocatorType& Allocator, PointerType const Element)
		{
			if constexpr (Private::HasDestroy<AllocatorType>::Value)
			{
				Allocator.Destroy(Element);
			}
			else
			{
				Element->~ElementType();
			}
		}

		template<typename... ArgTypes>
		static void ConstructQuantity(AllocatorType& Allocator,
			PointerType const Dest,
			const size_t Count,
			ArgTypes&&... Args)
		{
			if constexpr (Private::HasConstructQuantity<AllocatorType>::Value)
			{
				Allocator.ConstructQuantity(Dest, Count, std::forward<ArgTypes>(Args)...);
			}
			else
			{
				for (size_t i = 0; i < Count; ++i)
				{
					Construct(Allocator, Dest + i, std::forward<ArgTypes>(Args)...);
				}
			}
		}

		static void DestroyQuantity(AllocatorType& Allocator, PointerType const Dest, const size_t Count)
		{
			if constexpr (Private::HasDestroyQuantity<AllocatorType>::Value)
			{
				Allocator.DestroyQuantity(Dest, Count);
			}
			else
			{
				for (size_t i = 0; i < Count; ++i)
				{
					Destroy(Allocator, Dest + i);
				}
			}
		}

		static void ConstructQuantityFrom(AllocatorType& Allocator,
			PointerType const Dest,
			const size_t Count,
			PointerType const From)
		{
			if constexpr (Private::HasConstructQuantityFrom<AllocatorType>::Value)
			{
				Allocator.ConstructQuantityFrom(Dest, Count, From);
			}
			else
			{
				for (size_t i = 0; i < Count; ++i)
				{
					Construct(Allocator, Dest + i, std::move(From[i]));
				}
			}
		}
	};
}