#pragma once

//#include "Allocator.h"
//#include "AllocatorTraits.h"
//#include "AssertionMacros.h"

#include <vector>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InElementType, typename InAllocatorType = std::allocator<InElementType>>
	using TVector = std::vector<InElementType, InAllocatorType>;

	//template<typename InElementType, IsAllocator InAllocatorType = TAllocator<InElementType>>
	//class TVector final
	//{
	//public:
	//	using ElementType = InElementType;
	//	using AllocatorType = InAllocatorType;
	//	using AllocatorTypeTraits = IAllocatorTraits<AllocatorType>;
	//	using PointerType = ElementType*;
	//	using ConstPointerType = const ElementType*;
	//	using ReferenceType = ElementType&;
	//	using ConstReferenceType = const ElementType&;
	//	using IteratorType = PointerType;
	//	using ConstIteratorType = ConstPointerType;
	//	using ReverseIteratorType = IteratorType;
	//	using ConstReverseIteratorType = ConstIteratorType;
	//
	//	TVector() noexcept = default;
	//
	//	explicit TVector(const size_t Count,
	//		ConstReferenceType Value = ElementType(),
	//		const AllocatorType& Allocator = AllocatorType()) :
	//		Allocator(Allocator)
	//	{
	//		if (!Count) return;
	//
	//		Reserve(Count);
	//
	//		AllocatorTypeTraits::ConstructQuantity(this->Allocator, Array, Count, Value);
	//
	//		Size = Count;
	//	}
	//
	//	TVector(const TVector& Other) :
	//		Allocator(Other.Allocator),
	//		Size(Other.Size),
	//		Capacity(Other.Capacity)
	//	{
	//		Array = reinterpret_cast<PointerType>(AllocatorTypeTraits::Allocate(Allocator, Size));
	//
	//		try
	//		{
	//			AllocatorTypeTraits::ConstructQuantityFrom(Allocator, Array, Size, Other.Array);
	//		}
	//		catch (...)
	//		{
	//			AllocatorTypeTraits::Deallocate(Allocator, Array, Size * sizeof(ElementType));
	//			throw;
	//		}
	//	}
	//
	//	TVector(TVector&& Other) noexcept :
	//		Allocator(std::move(Other.Allocator)),
	//		Array(std::exchange(Other.Array, nullptr)),
	//		Size(std::exchange(Other.Size, 0)),
	//		Capacity(std::exchange(Other.Capacity, 0))
	//	{
	//	}
	//
	//	~TVector()
	//	{
	//		AllocatorTypeTraits::DestroyQuantity(Allocator, Array, Size);
	//		AllocatorTypeTraits::Deallocate(Allocator, Array, Capacity * sizeof(ElementType));
	//	}
	//
	//	TVector& operator=(const TVector& Other)
	//	{
	//		const AllocatorType OldAllocator = Allocator;
	//		Allocator = Other.Allocator;
	//		PointerType const NewArray = reinterpret_cast<PointerType>(AllocatorTypeTraits::Allocate(Allocator, Other.Size));
	//
	//		try
	//		{
	//			AllocatorTypeTraits::ConstructQuantityFrom(Allocator, NewArray, Other.Size, Other.Array);
	//		}
	//		catch (...)
	//		{
	//			AllocatorTypeTraits::Deallocate(Allocator, NewArray, Other.Size * sizeof(ElementType));
	//			Allocator = OldAllocator;
	//			throw;
	//		}
	//
	//		AllocatorTypeTraits::DestroyQuantity(Allocator, Array, Size);
	//		AllocatorTypeTraits::Deallocate(Allocator, Array, Capacity * sizeof(ElementType));
	//
	//		Array = NewArray;
	//		Size = Other.Size;
	//		Capacity = Other.Capacity;
	//
	//		return *this;
	//	}
	//
	//	TVector& operator=(TVector&& Other) noexcept
	//	{
	//		Allocator = std::move(Other.Allocator);
	//		Array = std::exchange(Other.Array, nullptr);
	//		Size = std::exchange(Other.Size, 0);
	//		Capacity = std::exchange(Other.Capacity, 0);
	//
	//		return *this;
	//	}
	//
	//	size_t GetSize() const
	//	{
	//		return Size;
	//	}
	//
	//	size_t GetCapacity() const
	//	{
	//		return Capacity;
	//	}
	//
	//	bool IsEmpty() const
	//	{
	//		return !Size;
	//	}
	//
	//	void Reserve(const size_t NewCapacity)
	//	{
	//		if (NewCapacity <= Capacity) return;
	//
	//		PointerType const NewArray = reinterpret_cast<PointerType>(AllocatorTypeTraits::Allocate(Allocator, NewCapacity));
	//
	//		if (Size)
	//		{
	//			try
	//			{
	//				AllocatorTypeTraits::ConstructQuantityFrom(Allocator, NewArray, Size, Array);
	//			}
	//			catch (...)
	//			{
	//				AllocatorTypeTraits::Deallocate(Allocator, NewArray, NewCapacity * sizeof(ElementType));
	//				throw;
	//			}
	//
	//			AllocatorTypeTraits::DestroyQuantity(Allocator, Array, Size);
	//		}
	//
	//		AllocatorTypeTraits::Deallocate(Allocator, Array, Capacity * sizeof(ElementType));
	//
	//		Array = NewArray;
	//		Capacity = NewCapacity;
	//	}
	//
	//	void Resize(const size_t NewSize, ConstReferenceType Value = ElementType())
	//	{
	//		if (NewSize > Capacity)
	//		{
	//			Reserve(NewSize);
	//
	//			AllocatorTypeTraits::ConstructQuantity(Allocator, Array + Size, NewSize - Size, Value);
	//		}
	//		else if (NewSize < Size)
	//		{
	//			AllocatorTypeTraits::DestroyQuantity(Allocator, Array + NewSize, Size - NewSize);
	//		}
	//
	//		Size = NewSize;
	//	}
	//
	//	void PushBack(ConstReferenceType Value)
	//	{
	//		EmplaceBack(Value);
	//	}
	//
	//	void PushBack(ElementType&& Value)
	//	{
	//		EmplaceBack(std::move(Value));
	//	}
	//
	//	template<typename... ArgTypes>
	//	void EmplaceBack(ArgTypes&&... Args)
	//	{
	//		if (Size == Capacity)
	//		{
	//			Reserve(GetCapacityGrowth());
	//		}
	//
	//		AllocatorTypeTraits::Construct(Allocator, Array + Size++, std::forward<ArgTypes>(Args)...);
	//	}
	//
	//	void PopBack() noexcept
	//	{
	//		BS_ASSERT(Size, "PopBack() on empty TVector");
	//
	//		AllocatorTypeTraits::Destroy(Allocator, Array + --Size);
	//	}
	//
	//	template<typename... ArgTypes>
	//	void Emplace(const size_t Position, ArgTypes&&... Args)
	//	{
	//		if (Position > Size) throw std::out_of_range("Index was out of range");
	//
	//		if (Position == Size)
	//		{
	//			EmplaceBack(std::forward<ArgTypes>(Args)...);
	//			return;
	//		}
	//
	//		if (Size == Capacity)
	//		{
	//			size_t NewCapacity = GetCapacityGrowth();
	//
	//			PointerType NewArray = reinterpret_cast<PointerType>(AllocatorTypeTraits::Allocate(Allocator, NewCapacity));
	//
	//			if (Size)
	//			{
	//				try
	//				{
	//					AllocatorTypeTraits::ConstructQuantityFrom(Allocator, NewArray, Position, Array);
	//
	//					PointerType const Dest = NewArray + Position;
	//					const size_t Count = Size - Position + 1;
	//					PointerType const From = Array + Position - 1;
	//
	//					AllocatorTypeTraits::ConstructQuantityFrom(Allocator, Dest, Count, From);
	//					AllocatorTypeTraits::Construct(Allocator, NewArray + Position, std::forward<ArgTypes>(Args)...);
	//				}
	//				catch (...)
	//				{
	//					AllocatorTypeTraits::Deallocate(Allocator, NewArray, NewCapacity * sizeof(ElementType));
	//					throw;
	//				}
	//
	//				for (size_t i = 0; i < Size; ++i)
	//				{
	//					AllocatorTypeTraits::Destroy(Allocator, Array + i);
	//				}
	//			}
	//
	//			AllocatorTypeTraits::Deallocate(Allocator, Array, NewCapacity * sizeof(ElementType));
	//
	//			Array = NewArray;
	//			Capacity = NewCapacity;
	//		}
	//		else
	//		{
	//			for (size_t i = Size; i > Position; --i)
	//			{
	//				Array[i] = std::move(Array[i - 1]);
	//			}
	//
	//			AllocatorTypeTraits::Construct(Allocator, Array + Position, std::forward<ArgTypes>(Args)...);
	//		}
	//
	//		++Size;
	//	}
	//
	//	template<typename... ArgTypes>
	//	void Emplace(ConstIteratorType const Position, ArgTypes&&... Args)
	//	{
	//		const size_t Idx = Position - Array;
	//
	//		Emplace(Idx, std::forward<ArgTypes>(Args)...);
	//	}
	//
	//	void Clear()
	//	{
	//		AllocatorTypeTraits::DestroyQuantity(Allocator, Array, Size);
	//		Size = 0;
	//	}
	//
	//	ReferenceType operator[](const size_t Position)
	//	{
	//		return Array[Position];
	//	}
	//
	//	ConstReferenceType operator[](const size_t Position) const
	//	{
	//		return Array[Position];
	//	}
	//
	//	ReferenceType At(const size_t Position)
	//	{
	//		if (Position >= Size) throw std::out_of_range("Index was out of range");
	//
	//		return Array[Position];
	//	}
	//
	//	ConstReferenceType At(const size_t Position) const
	//	{
	//		if (Position >= Size) throw std::out_of_range("Index was out of range");
	//
	//		return Array[Position];
	//	}
	//
	//private:
	//	AllocatorType Allocator = AllocatorType();
	//	PointerType Array = nullptr;
	//	size_t Size = 0;
	//	size_t Capacity = 0;
	//
	//	size_t GetCapacityGrowth()
	//	{
	//		return (size_t)((float)(!Size ? 1 : Size) * 1.5f);
	//	}
	//};
}
