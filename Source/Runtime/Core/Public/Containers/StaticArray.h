#pragma once

#include "Misc/AssertionMacros.h"
#include "CoreMisc.h"
#include "Platform/Platform.h"
#include "Templates/TypeTraits.h"

#include <array>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType, size_t Size>
	using TStaticArray = std::array<InElementType, Size>;

	//template <typename InElementType, size_t Size>
	//class TStaticArray
	//{
	//	static_assert(std::is_object_v<InElementType>, "Containers of non-object types are forbidden");
	//
	//public:
	//	using ElementType = InElementType;
	//	using PointerType = ElementType*;
	//	using ConstPointerType = const ElementType*;
	//	using ReferenceType = ElementType&;
	//	using ConstReferenceType = const ElementType&;
	//	using RandomAccessIteratorType = PointerType;
	//	using ConstRandomAccessIteratorType = ConstPointerType;
	//	using ReverseRandomAccessIteratorType = RandomAccessIteratorType;
	//	using ConstReverseRandomAccessIteratorType = ConstRandomAccessIteratorType;
	//
	//	constexpr TArray() = default;
	//
	//	constexpr TArray(const TArray<ElementType, Size>& Other)
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			Data[i]->~ElementType();
	//		}
	//
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			new(Data[i]) ElementType(Other.Data[i]);
	//		}
	//	}
	//
	//	constexpr TArray(TArray<ElementType, Size>&& Other) noexcept
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			Data[i]->~ElementType();
	//		}
	//
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			Data[i] = std::move(Other.Data[i]);
	//		}
	//	}
	//
	//	constexpr TArray(const std::initializer_list<ElementType>& InitializerList)
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			Data[i]->~ElementType();
	//		}
	//
	//		const size_t Count = Size > InitializerList.size() ? InitializerList.size() : Size;
	//
	//		PointerType InitListIt = InitializerList.begin();
	//
	//		for (size_t i = 0; i < Count; ++i)
	//		{
	//			new(Data[i]) ElementType(std::move(*InitListIt));
	//			++InitListIt;
	//		}
	//	}
	//
	//	constexpr ~TArray()
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			Data[i]->~ElementType();
	//		}
	//	}
	//
	//	NODISCARD FORCEINLINE ReferenceType operator=(const size_t Index)
	//	{
	//		return Data[Index];
	//	}
	//
	//	NODISCARD FORCEINLINE ConstReferenceType operator=(const size_t Index) const
	//	{
	//		return Data[Index];
	//	}
	//
	//	NODISCARD FORCEINLINE PointerType GetData() noexcept
	//	{
	//		return Data;
	//	}
	//
	//	NODISCARD FORCEINLINE ConstPointerType GetData() const noexcept
	//	{
	//		return Data;
	//	}
	//
	//	NODISCARD FORCEINLINE bool IsEmpty() const
	//	{
	//		return false;
	//	}
	//
	//	FORCEINLINE void RangeCheck(const size_t Index)
	//	{
	//		BS_ASSERT(Index >= 0 && Index < Size, "TStaticArray index out of bounds: index - {}, size - {}", Index, Size);
	//	}
	//
	//	NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const
	//	{
	//		return Index >= 0 && Index < Size;
	//	}
	//
	//	template<typename PredicateType>
	//	NODISCARD FORCEINLINE bool ContainsByPredicate(const PredicateType& Predicate)
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			if (Predicate(Data[i]))
	//			{
	//				return true;
	//			}
	//		}
	//
	//		return false;
	//	}
	//
	//	template<typename PredicateType>
	//	NODISCARD FORCEINLINE PointerType FindByPredicate(const PredicateType& Predicate)
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			ReferenceType Current = Data[i];
	//
	//			if (Predicate(Current))
	//			{
	//				return &Current;
	//			}
	//		}
	//
	//		return nullptr;
	//	}
	//
	//	template<typename PredicateType>
	//	NODISCARD FORCEINLINE ConstPointerType FindByPredicate(const PredicateType& Predicate) const
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			ConstReferenceType Current = Data[i];
	//
	//			if (Predicate(Current))
	//			{
	//				return &Current;
	//			}
	//		}
	//
	//		return nullptr;
	//	}
	//
	//	template<typename PredicateType>
	//	NODISCARD FORCEINLINE size_t IndexOfByPredicate(PredicateType Predicate) const
	//	{
	//		for (size_t i = 0; i < Size; ++i)
	//		{
	//			if (Predicate(Data[i]))
	//			{
	//				return i;
	//			}
	//		}
	//	}
	//
	//private:
	//	PointerType Data[Size];
	//};
}
