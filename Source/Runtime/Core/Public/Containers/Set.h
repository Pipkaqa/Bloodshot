#pragma once

#include "Allocators/Allocator.h"
#include "Allocators/AllocatorTraits.h"
#include "Containers/SparseArray.h"
#include "Misc/AssertionMacros.h"
#include "Misc/CoreMisc.h"
#include "Platform/Platform.h"
#include "Templates/Template.h"
#include "Templates/TypeHash.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	template<typename InKeyType, typename InValueType, bool bInAllowDuplicateKeys = false>
	struct TBaseKeyFuncs
	{
		using KeyType = InKeyType;
		using ValueType = InValueType;

		using KeyInitType = TCallTraits<KeyType>::ParamType;
		using ElementInitType = TCallTraits<ValueType>::ParamType;

		static constexpr bool bAllowDuplicateKeys = bInAllowDuplicateKeys;
	};

	template<typename InKeyType, bool bInAllowDuplicateKeys = false>
	struct TDefaultKeyFuncs : TBaseKeyFuncs<InKeyType, InKeyType, bInAllowDuplicateKeys>
	{
		using KeyType = InKeyType;
		using ValueType = InKeyType;

		using KeyInitType = TCallTraits<KeyType>::ConstPointerType;
		using ElementInitType = TCallTraits<ValueType>::ParamType;

		NODISCARD FORCEINLINE static KeyInitType GetSetKey(ElementInitType Element)
		{
			return Element;
		}

		NODISCARD FORCEINLINE static uint64_t GetKeyHash(KeyInitType Key)
		{
			return GetTypeHash(Key);
		}

		template<typename ComparableKey>
		NODISCARD FORCEINLINE static uint64_t GetKeyHash(ComparableKey Key)
		{
			return GetTypeHash(Key);
		}

		NODISCARD FORCEINLINE static bool Matches(KeyInitType Left, KeyInitType Right)
		{
			return Left == Right;
		}

		template<typename ComparableKey>
		NODISCARD FORCEINLINE static bool Matches(KeyInitType Left, ComparableKey Right)
		{
			return Left == Right;
		}
	};

	template<typename InElementType,
		typename InKeyFuncs = TDefaultKeyFuncs<InElementType>,
		IsAllocator InAllocatorType = FDefaultAllocator,
		IsAllocator InHashAllocatorType = FDefaultAllocator>
	class TSet
	{
	public:
		using ElementType = InElementType;
		using KeyFuncs = InKeyFuncs;
		using AllocatorType = InAllocatorType;
		using HashAllocatorType = InHashAllocatorType;

	private:
		static_assert(TAllocatorTraits<AllocatorType>::IsContiguous 
			&& TAllocatorTraits<HashAllocatorType>::IsContiguous,
			"TSet requires contiguous memory allocator");

		class FElement
		{
		public:
			template<typename... ArgTypes>
			FElement(ArgTypes&&... Args)
				: Value(std::forward<ArgTypes>(Args)...)
			{
			}

			mutable size_t NextIndex;
			mutable size_t HashIndex;
			ElementType Value;
		};

		using KeyInitType = typename KeyFuncs::KeyInitType;

	public:
		FORCEINLINE TSet() = default;

		FORCEINLINE TSet(const TSet& Other)
		{
			*this = Other;
		}

		FORCEINLINE TSet(TSet&& Other)
		{
			Move(*this, Other);
		}

		FORCEINLINE ~TSet()
		{
			HashSize = 0;
		}

		TSet& operator=(const TSet& Other)
		{
			if (this != &Other)
			{
				const size_t NewHashSize = Other.HashSize;
				HashAllocator.ResizeAllocation(0, NewHashSize, sizeof(size_t));
				memcpy(HashAllocator.GetAllocation(), Other.HashAllocator.GetAllocation(), NewHashSize * sizeof(size_t));
				HashSize = NewHashSize;
				Data = Other.Data;
			}
			return *this;
		}

	private:
		template<typename SetType>
		FORCEINLINE static void Move(SetType& To, SetType& From)
		{
			To.Data = std::move(From.Data);
			To.HashAllocator.Move(From.HashAllocator);
			To.HashSize = From.HashSize;
			From.HashSize = 0;
		}

	public:
		TSet& operator=(TSet&& Other)
		{
			if (this != &Other)
			{
				Move(*this, Other);
			}
			return *this;
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index)
		{
			return Data[Index].Value;
		}

		NODISCARD FORCEINLINE const ElementType& operator[](const size_t Index) const
		{
			return Data[Index].Value;
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Data.GetSize();
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return Data.IsEmpty();
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const noexcept
		{
			return Index != IReservedValues::NoneIndex;
		}

		FORCEINLINE void Reserve(const size_t NewCapacity)
		{
			if (NewCapacity > Data.GetSize())
			{
				const size_t DesiredHashSize = CalculateDesiredHashSize(NewCapacity);
				if (ShouldRehash(NewCapacity, DesiredHashSize))
				{
					Rehash(NewCapacity);
				}
				Data.Reserve(NewCapacity);
			}
		}

		template<typename... ArgTypes>
		size_t EmplaceByHash(const uint64_t KeyHash, ArgTypes&&... Args)
		{
			FSparseArrayAllocationInfo AllocationInfo = Data.AddUninitialized();
			FElement& Element = *new(AllocationInfo) FElement(std::forward<ArgTypes>(Args)...);
			size_t NewHashIndex = AllocationInfo.Index;
			if (!TryReplaceExisting(KeyHash, Element, NewHashIndex))
			{
				RehashOrLink(Element, NewHashIndex, KeyHash);
			}
			return NewHashIndex;
		}

		template<typename... ArgTypes>
		size_t Emplace(ArgTypes&&... Args)
		{
			FSparseArrayAllocationInfo AllocationInfo = Data.AddUninitialized();
			FElement& Element = *new(AllocationInfo) FElement(std::forward<ArgTypes>(Args)...);
			size_t NewHashIndex = AllocationInfo.Index;
			const uint64_t KeyHash = KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(Element.Value));
			if (!TryReplaceExisting(KeyHash, Element, NewHashIndex))
			{
				RehashOrLink(Element, NewHashIndex, KeyHash);
			}
			return NewHashIndex;
		}

		void Remove(const ElementType& Key)
		{
			const uint64_t KeyHash = KeyFuncs::GetKeyHash(Key);
			size_t* NextIndex = &GetBucketIndex(KeyHash);
			while (*NextIndex != IReservedValues::NoneIndex)
			{
				FElement& Element = Data[*NextIndex];
				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Element.Value), Key))
				{
					RemoveByIndex(*NextIndex);
					break;
				}
				else
				{
					NextIndex = &Element.NextIndex;
				}
			}
		}

		void RemoveByIndex(const size_t Index)
		{
			BS_CHECK(Data.IsValidIndex(Index));
			const FElement& ToRemove = Data[Index];
			size_t* NextIndexIt = &HashAllocator.GetAllocation()[ToRemove.HashIndex];
			while (true)
			{
				const size_t NextIndex = *NextIndexIt;
				BS_CHECK(NextIndex != IReservedValues::NoneIndex);
				if (NextIndex == Index)
				{
					*NextIndexIt = ToRemove.NextIndex;
					break;
				}
				NextIndexIt = &Data[NextIndex].NextIndex;
			}
			Data.RemoveAt(Index);
		}

		NODISCARD FORCEINLINE bool Contains(const ElementType& Key) const
		{
			const uint64_t KeyHash = KeyFuncs::GetKeyHash(Key);
			const size_t KeyIndex = FindIndexByHash(KeyHash, Key);
			return KeyIndex != IReservedValues::NoneIndex;
		}

		template<typename ComparableKey>
		NODISCARD ElementType* FindByHash(const uint64_t KeyHash, const ComparableKey& Key)
		{
			const size_t ElementIndex = FindIndexByHash(KeyHash, Key);
			if(ElementIndex != IReservedValues::NoneIndex)
			{
				return &Data[ElementIndex].Value;
			}
			return nullptr;
		}

		template<typename ComparableKey>
		NODISCARD const ElementType* FindByHash(const uint64_t KeyHash, const ComparableKey& Key) const
		{
			return const_cast<TSet*>(this)->FindByHash(KeyHash, Key);
		}

		NODISCARD FORCEINLINE ElementType* Find(KeyInitType Key)
		{
			const size_t ElementIndex = FindIndexByHash(KeyFuncs::GetKeyHash(Key), Key);
			return ElementIndex != IReservedValues::NoneIndex
				? &Data[ElementIndex].Value
				: nullptr;
		}

		NODISCARD FORCEINLINE const ElementType* Find(KeyInitType Key) const
		{
			return const_cast<TSet*>(this)->Find(Key);
		}

		NODISCARD ElementType& FindOrAddByHash(const uint64_t KeyHash, ElementType&& InElement)
		{
			const size_t ExistingIndex = FindIndexByHash(KeyHash, KeyFuncs::GetSetKey(InElement));
			const bool bIsAlreadyInSet = ExistingIndex != IReservedValues::NoneIndex;
			if (bIsAlreadyInSet) 
			{
				return Data[ExistingIndex].Value;
			}
			FSparseArrayAllocationInfo ElementAllocation = Data.AddUninitialized();
			FElement Element = *new(ElementAllocation) FElement(std::forward<ElementType>(InElement));
			RehashOrLink(Element, ElementAllocation.Index, KeyHash);
			return Element.Value;
		}

		NODISCARD FORCEINLINE ElementType& FindOrAdd(const ElementType& Element)
		{
			return FindOrAddByHash(KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(Element)), Element);
		}

		NODISCARD FORCEINLINE ElementType& FindOrAdd(ElementType&& Element)
		{
			return FindOrAddByHash(KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(Element)), std::move(Element));
		}

		FORCEINLINE void Clear()
		{
			Data.Clear();
			for (size_t i = 0; i < HashSize; ++i)
			{
				GetBucketIndex(i) = IReservedValues::NoneIndex;
			}
		}

		FORCEINLINE void Dispose()
		{
			Data.Dispose();
			HashAllocator.Dispose();
			HashSize = 0;
		}

	private:
		using DataType = TSparseArray<FElement, AllocatorType>;
		DataType Data;

		using MyHashAllocatorType = typename TAllocatorTraits<HashAllocatorType>::template ForElementType<size_t>;
		MyHashAllocatorType HashAllocator;

		size_t HashSize = 0;

		NODISCARD FORCEINLINE size_t GetHashIndex(const uint64_t KeyHash) const
		{
			return KeyHash & (HashSize - 1);
		}

		NODISCARD FORCEINLINE size_t& GetBucketIndex(const uint64_t KeyHash) const
		{
			return HashAllocator.GetAllocation()[GetHashIndex(KeyHash)];
		}
		
		template<typename ComparableKey>
		NODISCARD size_t FindIndexByHash(const uint64_t KeyHash, const ComparableKey& Key) const
		{
			if (!Data.GetSize())
			{
				return IReservedValues::NoneIndex;
			}

			size_t ElementIndex = GetBucketIndex(KeyHash);
			while (true)
			{
				if (ElementIndex == IReservedValues::NoneIndex)
				{
					return IReservedValues::NoneIndex;
				}

				if (KeyFuncs::Matches(KeyFuncs::GetSetKey(Data[ElementIndex].Value), Key))
				{
					return ElementIndex;
				}

				ElementIndex = Data[ElementIndex].NextIndex;
			}
		}

		bool TryReplaceExisting(const uint64_t KeyHash, FElement& Element, size_t& InOutNewHashIndex)
		{
			bool bAlreadyInSet = false;
			if (Data.GetSize() > 1)
			{
				const size_t ExistingIndex = FindIndexByHash(KeyHash, KeyFuncs::GetSetKey(Element.Value));
				bAlreadyInSet = ExistingIndex != IReservedValues::NoneIndex;
				if (bAlreadyInSet)
				{
					DestructElement(&Data[ExistingIndex].Value);
					MoveConstructElements<ElementType>(&Data[ExistingIndex].Value, &Element.Value, 1);
					DestructElement(&Element.Value);
					Data.RemoveAtUninitialized(InOutNewHashIndex);
					InOutNewHashIndex = ExistingIndex;
				}
			}
			return bAlreadyInSet;
		}

		FORCEINLINE void LinkElement(const FElement& Element, const size_t ElementIndex, const uint64_t KeyHash) const
		{
			size_t& ElementHashIndex = Element.HashIndex;
			ElementHashIndex = GetHashIndex(KeyHash);
			size_t& ElementBucketIndex = HashAllocator.GetAllocation()[ElementHashIndex];
			Element.NextIndex = ElementBucketIndex;
			ElementBucketIndex = ElementIndex;
		}

		FORCEINLINE void HashElement(const FElement& Element, const size_t ElementIndex) const
		{
			LinkElement(Element, ElementIndex, KeyFuncs::GetKeyHash(KeyFuncs::GetSetKey(Element.Value)));
		}

		void Rehash(const size_t NewHashSize)
		{
			BS_CHECK(std::has_single_bit(NewHashSize));
			HashAllocator.ResizeAllocation(HashSize, NewHashSize, sizeof(size_t));
			HashSize = NewHashSize;
			if (HashSize)
			{
				for (size_t i = 0; i < HashSize; ++i)
				{
					GetBucketIndex(i) = IReservedValues::NoneIndex;
				}

				for (typename DataType::FConstIterator ElementIt = Data.CreateConstIterator(); ElementIt; ++ElementIt)
				{
					HashElement(*ElementIt, ElementIt.GetIndex());
				}
			}
		}

		FORCEINLINE size_t CalculateDesiredHashSize(const size_t HashedElementCount) const
		{
			size_t Result = (HashedElementCount * 2) - 1;
			Result |= Result >> 1;
			Result |= Result >> 2;
			Result |= Result >> 4;
			Result |= Result >> 8;
			Result |= Result >> 16;
			Result |= Result >> 32;
			return Result + 1;
		}

		FORCEINLINE bool ShouldRehash(const size_t HashedElementCount, const size_t DesiredHashSize) const
		{
			return HashedElementCount > 0 && HashSize < DesiredHashSize;
		}

		FORCEINLINE bool ConditionalRehash(const size_t HashedElementCount)
		{
			const size_t DesiredHashSize = CalculateDesiredHashSize(HashedElementCount);
			const bool bShouldRehash = ShouldRehash(HashedElementCount, DesiredHashSize);
			if (bShouldRehash)
			{
				Rehash(DesiredHashSize);
			}
			return bShouldRehash;
		}

		FORCEINLINE void RehashOrLink(const FElement& Element, const size_t ElementIndex, const uint64_t KeyHash)
		{
			if (!ConditionalRehash(Data.GetSize()))
			{
				LinkElement(Element, ElementIndex, KeyHash);
			}
		}

	public:
		class FIterator
		{
			using UnderlyingIteratorType = DataType::FIterator;

		public:
			FORCEINLINE FIterator(TSet& InSet, const UnderlyingIteratorType& InIterator)
				: Set(InSet)
				, Iterator(InIterator)
			{
			}

			FORCEINLINE FIterator(const FIterator& Other) noexcept
				: Set(Other.Set)
				, Iterator(Other.Iterator)
			{
			}

			NODISCARD FORCEINLINE ElementType& operator*() const
			{
				return Iterator->Value;
			}

			NODISCARD FORCEINLINE ElementType* operator->() const
			{
				return &Iterator->Value;
			}

			FORCEINLINE FIterator operator++() noexcept
			{
				++Iterator;
				return *this;
			}

			FORCEINLINE FIterator operator++(int) noexcept
			{
				FIterator Temp = *this;
				++Iterator;
				return Temp;
			}

			FORCEINLINE bool operator==(const FIterator& Other) const
			{
				return Iterator == Other.Iterator && &Set == &Other.Set;
			}

			FORCEINLINE bool operator!=(const FIterator& Other) const
			{
				return Iterator != Other.Iterator || &Set != &Other.Set;
			}

			FORCEINLINE bool operator !() const
			{
				return !(bool)Iterator;
			}

			FORCEINLINE explicit operator bool() const
			{
				return (bool)Iterator;
			}

			NODISCARD FORCEINLINE size_t GetIndex() const noexcept
			{
				return Iterator.GetIndex();
			}

			FORCEINLINE void SetToEnd() noexcept
			{
				Iterator.SetToEnd();
			}

		private:
			TSet& Set;
			UnderlyingIteratorType Iterator;
		};

		class FConstIterator
		{
			using UnderlyingIteratorType = DataType::FConstIterator;

		public:
			FORCEINLINE FConstIterator(const TSet& InSet, const UnderlyingIteratorType& InIterator)
				: Set(InSet)
				, Iterator(InIterator)
			{
			}

			FORCEINLINE FConstIterator(const FConstIterator& Other) noexcept
				: Set(Other.Set)
				, Iterator(Other.Iterator)
			{
			}

			NODISCARD FORCEINLINE const ElementType& operator*() const
			{
				return Iterator->Value;
			}

			NODISCARD FORCEINLINE const ElementType* operator->() const
			{
				return &Iterator->Value;
			}

			FORCEINLINE FConstIterator operator++() noexcept
			{
				++Iterator;
				return *this;
			}

			FORCEINLINE FConstIterator operator++(int) noexcept
			{
				FConstIterator Temp = *this;
				++Iterator;
				return Temp;
			}

			FORCEINLINE bool operator==(const FConstIterator& Other) const
			{
				return Iterator == Other.Iterator && &Set == &Other.Set;
			}

			FORCEINLINE bool operator!=(const FConstIterator& Other) const
			{
				return Iterator != Other.Iterator || &Set != &Other.Set;
			}

			FORCEINLINE bool operator !() const
			{
				return !(bool)Iterator;
			}

			FORCEINLINE explicit operator bool() const
			{
				return (bool)Iterator;
			}

			NODISCARD FORCEINLINE size_t GetIndex() const noexcept
			{
				return Iterator.GetIndex();
			}

			FORCEINLINE void SetToEnd() noexcept
			{
				Iterator.SetToEnd();
			}

		private:
			const TSet& Set;
			UnderlyingIteratorType Iterator;
		};

		NODISCARD FORCEINLINE FIterator CreateIterator()
		{
			return FIterator(*this, Data.CreateIterator());
		}

		NODISCARD FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return FConstIterator(*this, Data.CreateConstIterator());
		}

		class FRangeForIterator
		{
			using UnderlyingIteratorType = DataType::FRangeForIterator;

		public:
			FORCEINLINE FRangeForIterator(const TSet& InSet, const UnderlyingIteratorType& InIterator)
				: Set(InSet)
				, Iterator(InIterator)
			{
			}

			FORCEINLINE FRangeForIterator(const FRangeForIterator& Other) noexcept
				: Set(Other.Set)
				, Iterator(Other.Iterator)
			{
			}

			NODISCARD FORCEINLINE ElementType& operator*() const
			{
				return Iterator->Value;
			}

			FORCEINLINE FRangeForIterator operator++() noexcept
			{
				++Iterator;
				return *this;
			}

			FORCEINLINE bool operator!=(const FRangeForIterator& Other) const
			{
				return Iterator != Other.Iterator || &Set != &Other.Set;
			}

		private:
			const TSet& Set;
			UnderlyingIteratorType Iterator;
		};

		class FRangeForConstIterator
		{
			using UnderlyingIteratorType = DataType::FRangeForConstIterator;

		public:
			FORCEINLINE FRangeForConstIterator(const TSet& InSet, const UnderlyingIteratorType& InIterator)
				: Set(InSet)
				, Iterator(InIterator)
			{
			}

			FORCEINLINE FRangeForConstIterator(const FRangeForConstIterator& Other) noexcept
				: Set(Other.Set)
				, Iterator(Other.Iterator)
			{
			}

			NODISCARD FORCEINLINE const ElementType& operator*() const
			{
				return Iterator->Value;
			}

			FORCEINLINE FRangeForConstIterator operator++() noexcept
			{
				++Iterator;
				return *this;
			}

			FORCEINLINE bool operator!=(const FRangeForConstIterator& Other) const
			{
				return Iterator != Other.Iterator || &Set != &Other.Set;
			}

		private:
			const TSet& Set;
			UnderlyingIteratorType Iterator;
		};

		// For internal usage only!
		FORCEINLINE FRangeForIterator begin() { return FRangeForIterator(*this, Data.begin()); }
		FORCEINLINE FRangeForIterator end() { return FRangeForIterator(*this, Data.end()); }
		FORCEINLINE FRangeForConstIterator begin() const { return FRangeForConstIterator(*this, Data.begin()); }
		FORCEINLINE FRangeForConstIterator end() const { return FRangeForConstIterator(*this, Data.end()); }
	};
}
