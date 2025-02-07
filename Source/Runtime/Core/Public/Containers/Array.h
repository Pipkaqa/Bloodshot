#pragma once

#include "Allocators/Allocator.h"
#include "Misc/AssertionMacros.h"
#include "Templates/MemoryOperations.h"

namespace Bloodshot
{
	// BSTODO: Work on optimization and code reuse

	template<typename ElementType, bool bReverse = false>
	class TCheckedPointerIterator
	{
	public:
		using PointerType = ElementType*;
		using ReferenceType = ElementType&;

		explicit TCheckedPointerIterator(PointerType Ptr, const size_t& Size)
			: Ptr(Ptr)
			, InitialSize(Size)
			, CurrentSize(Size)
		{
		}

		FORCEINLINE ReferenceType operator*() const
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
		PointerType Ptr;
		const size_t& InitialSize;
		size_t CurrentSize;
	};

	template <typename ContainerType, typename ElementType>
	class TIndexedContainerIterator
	{
	public:
		using PointerType = ElementType*;
		using ReferenceType = ElementType&;

		TIndexedContainerIterator(ContainerType& Container, const size_t StartIndex = 0)
			: Container(Container)
			, Index(StartIndex)
		{
		}

		FORCEINLINE ReferenceType operator*() const
		{
			return Container[Index];
		}

		FORCEINLINE PointerType operator->() const
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

		FORCEINLINE bool operator==(const TIndexedContainerIterator& Other) const
		{
			return &Container == &Other.Container && Index == Other.Index;
		}

		FORCEINLINE bool operator!=(const TIndexedContainerIterator& Other) const
		{
			return &Container != &Other.Container || Index != Other.Index;
		}

	private:
		ContainerType& Container;
		size_t Index;
	};

	template<typename InElementType, IsAllocator InAllocatorType = TAllocator<InElementType>>
	class TArray
	{
	public:
		using ElementType = InElementType;
		using AllocatorType = InAllocatorType;
		using PointerType = ElementType*;
		using ConstPointerType = const ElementType*;
		using ReferenceType = ElementType&;
		using ConstReferenceType = const ElementType&;
		using RValueReferenceType = ElementType&&;

		using FIterator = TIndexedContainerIterator<TArray, ElementType>;
		using FConstIterator = TIndexedContainerIterator<const TArray, const ElementType>;

		using FRangeBasedForIteratorType = TCheckedPointerIterator<ElementType, false>;
		using FRangeBasedForConstIteratorType = TCheckedPointerIterator<const ElementType, false>;
		using FRangeBasedForReverseIteratorType = TCheckedPointerIterator<ElementType, true>;
		using FRangeBasedForConstReverseIteratorType = TCheckedPointerIterator<const ElementType, true>;

		FORCEINLINE TArray() noexcept = default;

		FORCEINLINE TArray(const TArray& Other)
			: Allocator(Other.Allocator)
			, Size(Other.Size)
			, Capacity(Other.Capacity)
		{
			Data = Allocate(Capacity);
			ConstructElements<ElementType>(Data, Other.Data, Size);
		}

		FORCEINLINE TArray(TArray&& Other) noexcept :
			Allocator(std::move(Other.Allocator)),
			Data(std::exchange(Other.Data, nullptr)),
			Size(std::exchange(Other.Size, 0)),
			Capacity(std::exchange(Other.Capacity, 0))
		{
		}

		FORCEINLINE explicit TArray(const size_t Count,
			ConstReferenceType Value = ElementType(),
			const AllocatorType& Allocator = AllocatorType())
			: Allocator(Allocator)
		{
			if (!Count) return;

			Reserve(Count);

			for (size_t i = 0; i < Count; ++i)
			{
				new(Data + i) ElementType(Value);
			}

			Size = Count;
		}

		TArray(std::initializer_list<ElementType> InitiailizerList)
		{
			Size = InitiailizerList.size();
			Capacity = Size;

			Data = Allocate(Capacity);
			ConstructElements<ElementType>(Data, InitiailizerList.begin(), Size);
		}

		FORCEINLINE ~TArray()
		{
			DestructElements(Data, Size);
			Deallocate(Data, Capacity);
		}

		TArray& operator=(const TArray& Other)
		{
			DestructElements(Data, Size);
			Deallocate(Data, Capacity);

			Allocator = Other.Allocator;
			Size = Other.Size;
			Capacity = Other.Capacity;

			Data = Allocate(Capacity);
			ConstructElements<ElementType>(Data, Other.Data, Size);

			return *this;
		}

		TArray& operator=(TArray&& Other) noexcept
		{
			Allocator = std::move(Other.Allocator);
			Data = std::exchange(Other.Data, nullptr);
			Size = std::exchange(Other.Size, 0);
			Capacity = std::exchange(Other.Capacity, 0);

			return *this;
		}

		TArray& operator=(std::initializer_list<ElementType> InitiailizerList)
		{
			DestructElements(Data, Size);

			const size_t NewSize = InitiailizerList.size();

			if (NewSize > Capacity)
			{
				Deallocate(Data, Capacity);

				Size = NewSize;
				Capacity = Size;

				Allocate(Capacity);
			}
			else
			{
				Size = NewSize;
			}

			ConstructElements<ElementType>(Data, InitiailizerList.begin(), Size);

			return *this;
		}

		ReferenceType operator[](const size_t Index)
		{
			RangeCheck(Index);
			return Data[Index];
		}

		ConstReferenceType operator[](const size_t Index) const
		{
			RangeCheck(Index);
			return Data[Index];
		}

		NODISCARD FORCEINLINE PointerType GetData() noexcept
		{
			return Data;
		}

		NODISCARD FORCEINLINE ConstPointerType GetData() const noexcept
		{
			return Data;
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

		NODISCARD FORCEINLINE ReferenceType Last(const size_t IndexFromTheEnd = 0)
		{
			const size_t Index = Size - IndexFromTheEnd - 1;
			RangeCheck(Index);
			return Data[Index];
		}

		NODISCARD FORCEINLINE ConstReferenceType Last(const size_t IndexFromTheEnd = 0) const
		{
			const size_t Index = Size - IndexFromTheEnd - 1;
			RangeCheck(Index);
			return Data[Index];
		}

		FORCEINLINE void Init(const size_t Count, ConstReferenceType Element = ElementType())
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
			if (NewCapacity <= Capacity) return;

			PointerType OldData = Data;

			Data = Allocate(NewCapacity);
			MoveConstructElements(Data, OldData, Size);

			DestructElements(OldData, Size);
			Deallocate(OldData, Capacity);

			Capacity = NewCapacity;
		}

		void Resize(const size_t NewSize)
		{
			if (NewSize < Size)
			{
				DestructElements(Data + NewSize, Size - NewSize);
				return;
			}

			if (NewSize > Size)
			{
				Reserve(NewSize);

				DefaultConstructElements<ElementType>(Data + Size, NewSize - Size);
				Size = NewSize;
			}
		}

		void PushBack(ConstReferenceType Value)
		{
			EmplaceBack(Value);
		}

		void PushBack(RValueReferenceType Value)
		{
			EmplaceBack(std::move(Value));
		}

		void PopBack() noexcept
		{
			BS_ASSERT(Size, "TArray: PopBack() called on empty Array");
			DestructElement(Data + --Size);
		}

		template<typename... ArgTypes>
		void EmplaceBack(ArgTypes&&... Args)
		{
			if (Size == Capacity)
			{
				Reserve(GetCapacityGrowth());
			}

			new(Data + Size++) ElementType(std::forward<ArgTypes>(Args)...);
		}

		template<typename... ArgTypes>
		ReferenceType EmplaceBackGetRef(ArgTypes&&... Args)
		{
			EmplaceBack(std::forward<ArgTypes>(Args)...);
			return Data[Size - 1];
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
				PointerType OldData = Data;

				Data = Allocate(NewCapacity);
				MoveConstructElements(Data, OldData, Index);

				new(Data + Index) ElementType(std::forward<ArgTypes>(Args)...);

				MoveConstructElements(Data + Index + 1, Size - Index);
				DestructElements(OldData, Size);
				Deallocate(OldData, Capacity);

				Capacity = NewCapacity;
			}
			else
			{
				for (size_t i = Size; i > Index; --i)
				{
					Data[i] = std::move(Data[i - 1]);
				}

				new(Data + Index) ElementType(std::forward<ArgTypes>(Args)...);
			}

			++Size;
		}

		template<typename... ArgTypes>
		ReferenceType EmplaceAtGetRef(const size_t Index, ArgTypes&&... Args)
		{
			EmplaceAt(Index, std::forward<ArgTypes>(Args)...);
			return Data[Index];
		}

		void Append(const TArray& Other)
		{
			if (Capacity >= Size + Other.Size)
			{
				ConstructElements<ElementType>(Data + Size, Other.Data, Other.Size);
				Size = Size + Other.Size;
				return;
			}

			PointerType OldData = Data;
			const size_t OldCapacity = Capacity;

			Capacity = Size + Other.Size;
			Data = Allocate(Capacity);

			MoveConstructElements(Data, OldData, Size);
			ConstructElements<ElementType>(Data + Size, Other.Data, Other.Size);

			Deallocate(OldData, OldCapacity);
			Size = Capacity;
		}

		void Append(TArray&& Other)
		{
			if (Capacity >= Size + Other.Size)
			{
				MoveConstructElements(Data + Size, Other.Data, Other.Size);
				Size = Size + Other.Size;
				return;
			}

			PointerType OldData = Data;
			const size_t OldCapacity = Capacity;

			Capacity = Size + Other.Size;
			Data = Allocate(Capacity);

			MoveConstructElements(Data, OldData, Size);
			MoveConstructElements(Data + Size, Other.Data, Other.Size);

			Deallocate(OldData, OldCapacity);
			Size = Capacity;
		}

		void RemoveAt(const size_t Index)
		{
			RangeCheck(Index);

			DestructElement(Data + Index);

			for (size_t i = Index; i < Size; ++i)
			{
				Data[i] = std::move(Data[i + 1]);
			}

			--Size;
		}

		void RemoveAtSwap(const size_t Index)
		{
			RangeCheck(Index);

			ReferenceType Target = Data[Index];
			ReferenceType Last = Data[Size - 1];

			ElementType Temp = Target;
			Target = std::move(Last);
			Last = std::move(Temp);

			DestructElement(Data + Size - 1);
			--Size;
		}

		NODISCARD FORCEINLINE FIterator CreateIterator()
		{
			return FIterator(*this);
		}

		NODISCARD FORCEINLINE FConstIterator CreateConstIterator() const
		{
			return FConstIterator(*this);
		}

		FORCEINLINE void Shrink()
		{
			PointerType OldData = Data;
			const size_t OldCapacity = Capacity;

			Capacity = Size;
			Data = Allocate(Capacity);

			MoveConstructElements(Data, OldData, Size);
			Deallocate(OldData, OldCapacity);
		}

		FORCEINLINE void Clear()
		{
			DestructElements(Data, Size);
			Size = 0;
		}

		FORCEINLINE void Dispose()
		{
			Clear();
			Deallocate(Data, Capacity);

			Data = nullptr;
			Capacity = 0;
		}

		// For internal usage only!
		FORCEINLINE FRangeBasedForIteratorType begin() { return FRangeBasedForIteratorType(Data, Size); }
		FORCEINLINE FRangeBasedForIteratorType end() { return FRangeBasedForIteratorType(Data + Size, Size); }
		FORCEINLINE FRangeBasedForConstIteratorType begin() const { return FRangeBasedForConstIteratorType(Data, Size); }
		FORCEINLINE FRangeBasedForConstIteratorType end() const { return FRangeBasedForConstIteratorType(Data + Size, Size); }
		FORCEINLINE FRangeBasedForReverseIteratorType rbegin() { return FRangeBasedForReverseIteratorType(Data + Size, Size); }
		FORCEINLINE FRangeBasedForReverseIteratorType rend() { return FRangeBasedForReverseIteratorType(Data, Size); }
		FORCEINLINE FRangeBasedForConstReverseIteratorType rbegin() const { return FRangeBasedForConstReverseIteratorType(Data + Size, Size); }
		FORCEINLINE FRangeBasedForConstReverseIteratorType rend() const { return FRangeBasedForConstReverseIteratorType(Data, Size); }

		FIterator Find(ConstReferenceType Value)
		{
			for (FIterator It = CreateIterator(); It; ++It)
			{
				if (*It == Value)
				{
					return It;
				}
			}
		}

	private:
		AllocatorType Allocator = AllocatorType();
		PointerType Data = nullptr;
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

		NODISCARD FORCEINLINE PointerType Allocate(const size_t Count)
		{
			return ReinterpretCast<PointerType>(Allocator.Allocate(Count));
		}

		FORCEINLINE void Deallocate(void* const Ptr, const size_t Count)
		{
			Allocator.Deallocate(Ptr, sizeof(ElementType) * Count);
		}
	};
}
