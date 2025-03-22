#pragma once

#include "Allocators/Allocator.h"
#include "Allocators/AllocatorTraits.h"
#include "Misc/AssertionMacros.h"
#include "Misc/CoreMisc.h"
#include "Object/Object.h"
#include "Platform/Platform.h"
#include "Templates/MemoryOperations.h"

#include <cmath>

namespace Bloodshot
{
	template<typename ContainerType, typename ElementType>
	class TIndexedContainerIterator
	{
	public:
		TIndexedContainerIterator(ContainerType& Container, const size_t StartIndex = 0)
			: Container(Container)
			, Index(StartIndex)
		{
		}

		FORCEINLINE ElementType& operator*() const
		{
			return Container[Index];
		}

		FORCEINLINE ElementType* operator->() const
		{
			return &Container[Index];
		}

		TIndexedContainerIterator& operator++()
		{
			++Index;
			return *this;
		}

		TIndexedContainerIterator operator++(int)
		{
			TIndexedContainerIterator Temp(*this);
			++Index;
			return Temp;
		}

		TIndexedContainerIterator& operator--()
		{
			--Index;
			return *this;
		}

		TIndexedContainerIterator operator--(int)
		{
			TIndexedContainerIterator Temp(*this);
			--Index;
			return Temp;
		}

		TIndexedContainerIterator& operator+=(const size_t Offset)
		{
			Index += Offset;
			return *this;
		}

		TIndexedContainerIterator operator+(const size_t Offset) const
		{
			TIndexedContainerIterator Temp(*this);
			return Temp += Offset;
		}

		TIndexedContainerIterator& operator-=(const size_t Offset)
		{
			return *this += -Offset;
		}

		TIndexedContainerIterator operator-(const size_t Offset) const
		{
			TIndexedContainerIterator Temp(*this);
			return Temp -= Offset;
		}

		FORCEINLINE explicit operator bool() const
		{
			return Container.IsValidIndex(Index);
		}

		FORCEINLINE bool operator==(const TIndexedContainerIterator& Other) const
		{
			return &Container == &Other.Container && Index == Other.Index;
		}

		FORCEINLINE bool operator!=(const TIndexedContainerIterator& Other) const
		{
			return &Container != &Other.Container || Index != Other.Index;
		}

		NODISCARD FORCEINLINE size_t GetIndex() const
		{
			return Index;
		}

		FORCEINLINE void SetToEnd()
		{
			Index = Container.GetSize();
		}

		FORCEINLINE void RemoveCurrent()
		{
			Container.RemoveAt(Index);
			--Index;
		}

		FORCEINLINE void RemoveSwapCurrent()
		{
			Container.RemoveAtSwap(Index);
			--Index;
		}

		FORCEINLINE void Reset()
		{
			Index = 0;
		}

	private:
		ContainerType& Container;
		size_t Index;
	};

	template<typename ElementType, bool bReverse = false>
	class TCheckedPointerIterator
	{
	public:
		explicit TCheckedPointerIterator(ElementType* Ptr, const size_t& Size)
			: Ptr(Ptr)
			, InitialSize(Size)
			, CurrentSize(Size)
		{
		}

		FORCEINLINE ElementType& operator*() const
		{
			if constexpr (bReverse)
			{
				return *(Ptr - 1);
			}
			else
			{
				return *Ptr;
			}
		}

		FORCEINLINE TCheckedPointerIterator& operator++()
		{
			if constexpr (bReverse)
			{
				--Ptr;
			}
			else
			{
				++Ptr;
			}

			return *this;
		}

		FORCEINLINE TCheckedPointerIterator& operator--()
		{
			if constexpr (bReverse)
			{
				++Ptr;
			}
			else
			{
				--Ptr;
			}

			return *this;
		}

		FORCEINLINE bool operator!=(const TCheckedPointerIterator& Other) const
		{
			BS_ASSERT(CurrentSize == InitialSize, "TCheckedPointerIterator: Data has changed during range-based for loop iteration");
			return Ptr != Other.Ptr;
		}

	private:
		ElementType* Ptr;
		const size_t& InitialSize;
		size_t CurrentSize;
	};

	template<typename InElementType, IsAllocator InAllocatorType = FDefaultAllocator>
	class TArray
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;
		using ElementAllocatorType = typename TAllocatorTraits<InAllocatorType>::template ForElementType<ElementType>;

		static_assert(IsContiguousAllocator<ElementAllocatorType>, "TArray requires contiguous memory allocator");

		FORCEINLINE TArray() noexcept = default;

		FORCEINLINE TArray(const TArray& Other)
			: Size(Other.Size)
		{
			const size_t NewCapacity = Other.Capacity;
			if (NewCapacity > Capacity)
			{
				ResizeAllocation(0, NewCapacity);
				Capacity = NewCapacity;
			}
			ConstructElements<ElementType>(GetData(), Other.GetData(), Size);
		}

		FORCEINLINE TArray(TArray&& Other) noexcept
			: Size(std::exchange(Other.Size, 0))
		{
			Allocator.Move(Other.Allocator);
			Capacity = std::exchange(Other.Capacity, Other.Allocator.GetInitialCapacity());
		}

		FORCEINLINE TArray(const ElementAllocatorType& Allocator)
			: Allocator(Allocator)
		{
		}

		FORCEINLINE TArray(ElementAllocatorType&& Allocator)
			: Allocator(std::move(Allocator))
		{
		}

		FORCEINLINE TArray(std::initializer_list<ElementType> InitList)
			: Size(InitList.size())
			, Capacity(InitList.size())
		{
			ResizeAllocation(0, Size);
			ConstructElements<ElementType>(GetData(), InitList.begin(), Size);
		}

		FORCEINLINE ~TArray()
		{
			Dispose();
		}

		TArray& operator=(const TArray& Other)
		{
			DestructElements(GetData(), Size);
			Size = Other.Size;
			const size_t NewCapacity = Other.Capacity;
			if (NewCapacity > Capacity)
			{
				ResizeAllocation(Capacity, NewCapacity);
				Capacity = NewCapacity;
			}
			ConstructElements<ElementType>(GetData(), Other.GetData(), Size);
			return *this;
		}

		TArray& operator=(TArray&& Other) noexcept
		{
			Allocator.Move(Other.Allocator);
			Size = std::exchange(Other.Size, 0);
			Capacity = std::exchange(Other.Capacity, Other.Allocator.GetInitialCapacity());
			return *this;
		}

		TArray& operator=(std::initializer_list<ElementType> InitList)
		{
			DestructElements(GetData(), Size);
			Size = InitList.size();
			const size_t NewCapacity = Size;
			if (NewCapacity > Capacity)
			{
				ResizeAllocation(Capacity, NewCapacity);
				Capacity = NewCapacity;
			}
			ConstructElements<ElementType>(GetData(), InitList.begin(), Size);
			return *this;
		}

		NODISCARD FORCEINLINE ElementType& operator[](const size_t Index)
		{
			RangeCheck(Index);
			return GetData()[Index];
		}

		NODISCARD FORCEINLINE const ElementType& operator[](const size_t Index) const
		{
			RangeCheck(Index);
			return GetData()[Index];
		}

		NODISCARD FORCEINLINE bool operator==(const TArray& Other) const noexcept
		{
			if (Size != Other.Size)
			{
				return false;
			}

			const ElementType* const Data = GetData();
			const ElementType* const OtherData = Other.GetData();

			for (size_t i = 0; i < Size; ++i)
			{
				if (Data[i] != OtherData[i])
				{
					return false;
				}
			}

			return true;
		}

		NODISCARD FORCEINLINE ElementAllocatorType& GetAllocator() noexcept
		{
			return Allocator;
		}

		NODISCARD FORCEINLINE const ElementAllocatorType& GetAllocator() const noexcept
		{
			return Allocator;
		}

		NODISCARD FORCEINLINE ElementType* GetData() noexcept
		{
			return Allocator.GetAllocation();
		}

		NODISCARD FORCEINLINE const ElementType* GetData() const noexcept
		{
			return Allocator.GetAllocation();
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Size;
		}

		NODISCARD FORCEINLINE size_t GetCapacity() const noexcept
		{
			return Capacity;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return !Size;
		}

		NODISCARD FORCEINLINE bool IsValidIndex(const size_t Index) const noexcept
		{
			return Index >= 0 && Index < Size;
		}

		NODISCARD FORCEINLINE ElementType& Last(const size_t IndexFromTheEnd = 0)
		{
			const size_t Index = Size - IndexFromTheEnd - 1;
			RangeCheck(Index);
			return GetData()[Index];
		}

		NODISCARD FORCEINLINE const ElementType& Last(const size_t IndexFromTheEnd = 0) const
		{
			const size_t Index = Size - IndexFromTheEnd - 1;
			RangeCheck(Index);
			return GetData()[Index];
		}

		FORCEINLINE void Init(const size_t Count, const ElementType& Element = ElementType())
		{
			Clear();
			Reserve(Count);
			for (size_t i = 0; i < Count; ++i)
			{
				PushBack(Element);
			}
		}

		void Reserve(const size_t NewCapacity)
		{
			if (NewCapacity <= Capacity)
			{
				return;
			}
			ElementType* const OldData = ExchangeAllocation(Capacity, NewCapacity);
			MoveConstructElements(GetData(), OldData, Size);
			DestructElements(OldData, Size);
			ConsumeAllocation(OldData);
			Capacity = NewCapacity;
		}

		void Resize(const size_t NewSize)
		{
			if (NewSize < Size)
			{
				DestructElements(GetData() + NewSize, Size - NewSize);
			}
			else if (NewSize > Size)
			{
				Reserve(NewSize);
				DefaultConstructElements<ElementType>(GetData() + Size, NewSize - Size);
			}
			Size = NewSize;
		}

		FORCEINLINE void PushBack(const ElementType& Value)
		{
			EmplaceBack(Value);
		}

		FORCEINLINE void PushBack(ElementType&& Value)
		{
			EmplaceBack(std::move(Value));
		}

		FORCEINLINE void PopBack() noexcept
		{
			BS_ASSERT(Size, "TArray: PopBack() called on empty Array");
			DestructElement(GetData() + --Size);
		}

		template<typename... ArgTypes>
		void EmplaceBack(ArgTypes&&... Args)
		{
			const size_t Index = AddUninitialized();
			new(GetData() + Index) ElementType(std::forward<ArgTypes>(Args)...);
		}

		template<typename... ArgTypes>
		NODISCARD ElementType& EmplaceBackGetRef(ArgTypes&&... Args)
		{
			EmplaceBack(std::forward<ArgTypes>(Args)...);
			return GetData()[Size - 1];
		}

		template<typename... ArgTypes>
		void EmplaceAt(const size_t Index, ArgTypes&&... Args)
		{
			BS_ASSERT(Index >= 0 && Index <= Size, "TArray::EmplaceAt: index out of bounds, index - {}, size - {}", Index, Size);

			if (Index == Size)
			{
				EmplaceBack(std::forward<ArgTypes>(Args)...);
				return;
			}

			if (Size == Capacity)
			{
				const size_t NewCapacity = GetCapacityGrowth();

				ElementType* const OldData = ExchangeAllocation(Capacity, NewCapacity);
				Capacity = NewCapacity;
				ElementType* const Data = GetData();

				MoveConstructElements(Data, OldData, Index);
				new(Data + Index) ElementType(std::forward<ArgTypes>(Args)...);
				MoveConstructElements(Data + Index + 1, Size - Index);

				ConsumeAllocation(OldData);
			}
			else
			{
				ElementType* const Data = GetData();
				for (size_t i = Size; i > Index; --i)
				{
					Data[i] = std::move(Data[i - 1]);
				}
				new(Data + Index) ElementType(std::forward<ArgTypes>(Args)...);
			}

			++Size;
		}

		template<typename... ArgTypes>
		NODISCARD ElementType& EmplaceAtGetRef(const size_t Index, ArgTypes&&... Args)
		{
			EmplaceAt(Index, std::forward<ArgTypes>(Args)...);
			return GetData()[Index];
		}

		FORCEINLINE size_t AddUninitialized()
		{
			if (Size == Capacity)
			{
				Reserve(GetCapacityGrowth());
			}
			return Size++;
		}

		FORCEINLINE size_t AddUninitialized(const size_t Count)
		{
			const size_t OldSize = Size;
			const size_t NewSize = OldSize + Count;
			Reserve(NewSize);
			Size = NewSize;
			return OldSize;
		}

		void Append(const TArray& Other)
		{
			const size_t NewCapacity = Size + Other.Size;
			if (Capacity >= NewCapacity)
			{
				ConstructElements<ElementType>(GetData() + Size, Other.GetData(), Other.Size);
				Size += Other.Size;
				return;
			}

			ElementType* const OldData = ExchangeAllocation(Capacity, NewCapacity);
			Capacity = NewCapacity;
			ElementType* const Data = GetData();

			MoveConstructElements(Data, OldData, Size);
			ConstructElements<ElementType>(Data + Size, Other.GetData(), Other.Size);

			ConsumeAllocation(OldData);
			Size = NewCapacity;
		}

		void Append(TArray&& Other)
		{
			const size_t NewCapacity = Size + Other.Size;
			if (Capacity >= NewCapacity)
			{
				ConstructElements<ElementType>(GetData() + Size, Other.GetData(), Other.Size);
				Size += Other.Size;
				return;
			}

			ElementType* const OldData = ExchangeAllocation(Capacity, NewCapacity);
			Capacity = NewCapacity;
			ElementType* const Data = GetData();

			MoveConstructElements(Data, OldData, Size);
			MoveConstructElements(Data + Size, Other.GetData(), Other.Size);

			ConsumeAllocation(OldData);
			Size = NewCapacity;
		}

		void RemoveAt(const size_t Index, const size_t Count)
		{
			RangeCheck(Index, Count);
			ElementType* const Dest = GetData() + Index;
			DestructElements(Dest, Count);
			MoveConstructElements(Dest, Dest + Count, Size - Count);
			Size -= Count;
		}

		FORCEINLINE void RemoveAt(const size_t Index)
		{
			RemoveAt(Index, 1);
		}

		template<typename PredicateType>
		void RemoveByPredicate(PredicateType&& Predicate)
		{
			if (!Size)
			{
				return;
			}
			int64_t ReadIndex = 0;
			int64_t WriteIndex = Size - 1;
			ElementType* const Data = GetData();
			do
			{
				while ((bool)Predicate(Data[ReadIndex]) && ReadIndex - 1 < WriteIndex)
				{
					ElementType* const Target = Data + ReadIndex;
					DestructElement(Target);
					if (ReadIndex < WriteIndex)
					{
						ElementType* const Last = Data + WriteIndex;
						MoveConstructElements(Target, Last, 1);
					}
					--WriteIndex;
				}
				++ReadIndex;
			} while (ReadIndex < WriteIndex);
			Size = WriteIndex + 1;
		}

		void RemoveAtSwap(const size_t Index, const size_t Count)
		{
			RangeCheck(Index, Count);
			ElementType* const Data = GetData();
			ElementType* const Dest = Data + Index;
			DestructElements(Dest, Count);
			const size_t ElementsAfterHole = Size - Index - Count;
			const size_t ElementsToMoveIntoHole = std::min(Count, ElementsAfterHole);
			if (ElementsToMoveIntoHole)
			{
				MoveConstructElements(Dest, Data + (Size - ElementsToMoveIntoHole), ElementsToMoveIntoHole);
			}
			Size -= Count;
		}

		void RemoveAtSwap(const size_t Index)
		{
			RangeCheck(Index);
			ElementType* const Data = GetData();
			ElementType* const Target = Data + Index;
			DestructElement(Target);
			ElementType* const Last = Data + Size - 1;
			MoveConstructElements(Target, Last, 1);
			--Size;
		}

		FORCEINLINE void Shrink()
		{
			ResizeAllocation(Capacity, Size);
			Capacity = Size;
		}

		FORCEINLINE void Clear()
		{
			DestructElements(GetData(), Size);
			Size = 0;
		}

		FORCEINLINE void Dispose()
		{
			Clear();
			Allocator.Dispose();
			Capacity = 0;
		}

		NODISCARD size_t Find(const ElementType& Value) const
		{
			const ElementType* const Data = GetData();
			for (size_t i = 0; i < Size; ++i)
			{
				if (Data[i] == Value)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		template<IsObject SearchType>
		NODISCARD bool FindByClass(SearchType** const OutElement = nullptr, size_t* const OutIndex = nullptr) const
		{
			const FClass* const SearchClass = SearchType::StaticClass();
			const ElementType* const Data = GetData();
			for (size_t i = 0; i < Size; ++i)
			{
				const ElementType& Element = Data[i];
				if (Element->IsA(SearchClass))
				{
					if (OutElement)
					{
						*OutElement = const_cast<SearchType*>(reinterpret_cast<const SearchType*>(Element));
					}
					if (OutIndex)
					{
						*OutIndex = i;
					}
					return true;
				}
			}
			return false;
		}

		using FIterator = TIndexedContainerIterator<TArray, ElementType>;
		using FConstIterator = TIndexedContainerIterator<const TArray, const ElementType>;

		using FRangeBasedForIteratorType = TCheckedPointerIterator<ElementType, false>;
		using FRangeBasedForConstIteratorType = TCheckedPointerIterator<const ElementType, false>;
		using FRangeBasedForReverseIteratorType = TCheckedPointerIterator<ElementType, true>;
		using FRangeBasedForConstReverseIteratorType = TCheckedPointerIterator<const ElementType, true>;

		NODISCARD FORCEINLINE FIterator CreateIterator()
		{
			return FIterator(*this);
		}

		NODISCARD FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return FConstIterator(*this);
		}

		// For internal usage only!
		FORCEINLINE FRangeBasedForIteratorType begin() { return FRangeBasedForIteratorType(GetData(), Size); }
		FORCEINLINE FRangeBasedForIteratorType end() { return FRangeBasedForIteratorType(GetData() + Size, Size); }
		FORCEINLINE FRangeBasedForConstIteratorType begin() const { return FRangeBasedForConstIteratorType(GetData(), Size); }
		FORCEINLINE FRangeBasedForConstIteratorType end() const { return FRangeBasedForConstIteratorType(GetData() + Size, Size); }
		FORCEINLINE FRangeBasedForReverseIteratorType rbegin() { return FRangeBasedForReverseIteratorType(GetData() + Size, Size); }
		FORCEINLINE FRangeBasedForReverseIteratorType rend() { return FRangeBasedForReverseIteratorType(GetData(), Size); }
		FORCEINLINE FRangeBasedForConstReverseIteratorType rbegin() const { return FRangeBasedForConstReverseIteratorType(GetData() + Size, Size); }
		FORCEINLINE FRangeBasedForConstReverseIteratorType rend() const { return FRangeBasedForConstReverseIteratorType(GetData(), Size); }

	private:
		ElementAllocatorType Allocator;
		size_t Size = 0;
		size_t Capacity = 0;

		NODISCARD FORCEINLINE size_t GetCapacityGrowth()
		{
			return (size_t)std::ceil((float)Capacity + (!Capacity ? 1 : Capacity) * 1.5f);
		}

		FORCEINLINE void RangeCheck(const size_t Index) const
		{
			BS_ASSERT(Index >= 0 && Index < Size, "TArray: index out of bounds, index - {}, size - {}", Index, Size);
		}

		FORCEINLINE void RangeCheck(const size_t Index, const size_t Count) const
		{
			BS_ASSERT(Index >= 0 && Index + Count < Size, "TArray: index out of bounds, index - {}, count - {} size - {}", Index, Count, Size);
		}

		NODISCARD FORCEINLINE ElementType* ExchangeAllocation(const size_t CurrentSize, const size_t NewSize)
		{
			return Allocator.ExchangeAllocation(CurrentSize, NewSize, sizeof(ElementType));
		}

		FORCEINLINE void ConsumeAllocation(ElementType* const Data) const noexcept
		{
			Allocator.ConsumeAllocation((void*)Data);
		}

		FORCEINLINE void ResizeAllocation(const size_t CurrentSize, const size_t NewSize)
		{
			Allocator.ResizeAllocation(CurrentSize, NewSize, sizeof(ElementType));
		}
	};
}
