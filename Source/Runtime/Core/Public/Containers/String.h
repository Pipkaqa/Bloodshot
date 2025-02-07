#pragma once

//#include "Allocators/Allocator.h"
//#include "Allocators/AllocatorTraits.h"
//#include "Misc/Casts.h"
//#include "CoreMisc.h"
//#include "Templates/MemoryOperations.h"

#include <string>

namespace Bloodshot
{
	// BSTODO: Implement

	template<typename InAllocatorType = std::allocator<char>>
	using TString = std::basic_string<char, std::char_traits<char>, InAllocatorType>;

	using FString = TString<>;

	using FStringView = std::string_view;

	namespace Private
	{
		template<typename ElementType, typename AllocatorType>
		void FormatHelper(FString& OutString, FStringView& OutFormatStr, ElementType&& Value)
		{
			const size_t OpenBracketIndex = OutFormatStr.find('{');
			if (OpenBracketIndex == FString::npos) return;

			const size_t CloseBracketIndex = OutFormatStr.find('}', OpenBracketIndex + 1);
			if (CloseBracketIndex == FString::npos) return;

			OutString.append(OutFormatStr.substr(0, OpenBracketIndex));
			OutString.append(std::move(Value));

			OutFormatStr = OutFormatStr.substr(CloseBracketIndex + 1);
		}

		template<typename... ArgTypes>
		FString Format(FStringView FormatStr, ArgTypes&&... Args)
		{
			FString Result;
			(FormatHelper(Result, FormatStr, std::forward<ArgTypes>(Args)), ...);
			Result.append(FormatStr);
			return Result;
		}
	}

	//template<template<typename> typename InAllocatorType = TAllocator>
	//class TString
	//{
	//public:
	//	using ElementType = char;
	//	using AllocatorType = InAllocatorType<ElementType>;
	//	using AllocatorTraits = IAllocatorTraits<AllocatorType>;
	//	using PointerType = ElementType*;
	//	using ConstPointerType = const ElementType*;
	//	using ReferenceType = ElementType&;
	//	using ConstReferenceType = const ElementType&;
	//
	//	FORCEINLINE TString() noexcept = default;
	//
	//	FORCEINLINE TString(const TString& Other)
	//		: Allocator(Other.Allocator)
	//		, Size(Other.Size)
	//		, Capacity(Other.Capacity)
	//	{
	//		InitCopy(Other.Data);
	//	}
	//
	//	FORCEINLINE TString(TString&& Other) noexcept
	//		: Allocator(std::move(Other.Allocator))
	//		, Data(Other.Data)
	//		, Size(Other.Size)
	//		, Capacity(Other.Capacity)
	//	{
	//		Other.Data = nullptr;
	//		Other.Size = 0;
	//		Other.Capacity = 0;
	//	}
	//
	//	FORCEINLINE TString(const char* Other)
	//	{
	//		Size = strlen(Other);
	//		Capacity = Size;
	//
	//		InitCopy(Other);
	//	}
	//
	//	FORCEINLINE explicit TString(const AllocatorType& Allocator)
	//		: Allocator(Allocator)
	//	{
	//	}
	//
	//	FORCEINLINE ~TString()
	//	{
	//		Deallocate(Data, Capacity);
	//	}
	//
	//	NODISCARD FORCEINLINE TString& operator=(const TString& Other) noexcept
	//	{
	//		Deallocate(Data, Capacity);
	//
	//		Allocator = Other.Allocator;
	//		Capacity = Other.Capacity;
	//		Size = Other.Size;
	//
	//		InitCopy(Other.Data);
	//	}
	//
	//	NODISCARD FORCEINLINE TString& operator=(TString&& Other) noexcept
	//	{
	//		Allocator = std::move(Other.Allocator);
	//		Data = Other.Data;
	//		Size = Other.Size;
	//		Capacity = Other.Capacity;
	//
	//		Other.Data = nullptr;
	//		Other.Size = 0;
	//		Other.Capacity = 0;
	//	}
	//
	//	NODISCARD FORCEINLINE size_t GetSize() const noexcept
	//	{
	//		return Size;
	//	}
	//
	//	NODISCARD FORCEINLINE size_t GetCapacity() const noexcept
	//	{
	//		return Capacity;
	//	}
	//
	//	NODISCARD FORCEINLINE bool IsEmpty() const noexcept
	//	{
	//		return Size == 0;
	//	}
	//
	//	void Reserve(const size_t NewCapacity)
	//	{
	//		if (NewCapacity <= Capacity) return;
	//
	//		PointerType NewPtr = Allocate(NewCapacity);
	//
	//		if (Size) strncpy(NewPtr, Data, Size);
	//		Deallocate(Data, Capacity);
	//
	//		Data = NewPtr;
	//		Capacity = NewCapacity;
	//	}
	//
	//	void Resize(const size_t NewSize, ElementType Value = ElementType())
	//	{
	//		if (NewSize > Capacity)
	//		{
	//			Reserve(NewSize);
	//			strnset(Data + Size, Value, NewSize - Size);
	//		}
	//		else if (NewSize < Size)
	//		{
	//			strnset(Data + NewSize, Value, Size - NewSize);
	//		}
	//
	//		Size = NewSize;
	//	}
	//
	//
	//
	//private:
	//	AllocatorType Allocator = AllocatorType();
	//	PointerType Data = nullptr;
	//	size_t Size = 0;
	//	size_t Capacity = 0;
	//
	//	FORCEINLINE void InitCopy(PointerType Other)
	//	{
	//		Data = Allocate(Capacity);
	//		strncpy(Data, Other, Capacity);
	//	}
	//
	//	NODISCARD FORCEINLINE PointerType Allocate(const size_t Count)
	//	{
	//		return ReinterpretCast<PointerType>(Allocator.Allocate(Count));
	//	}
	//
	//	FORCEINLINE void Deallocate(void* const Ptr, const size_t Count)
	//	{
	//		Allocator.Deallocate(Ptr, sizeof(ElementType) * Count);
	//	}
	//};
	//
	//using FString = TString<>;
}