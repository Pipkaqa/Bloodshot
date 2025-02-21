#pragma once

#include "Containers/StringFwd.h"
#include "CoreMisc.h"
#include "Platform/Platform.h"
#include "Templates/IsCharRange.h"
#include "Templates/Template.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	template<typename InCharType>
	class TStringView
	{
	public:
		using ElementType = InCharType;

		FORCEINLINE TStringView() = default;

		FORCEINLINE TStringView(const TStringView& Other) noexcept
			: Data(Other.Data)
			, Size(Other.Size)
		{
		}

		FORCEINLINE TStringView(TStringView&& Other) noexcept
			: Data(Other.Data)
			, Size(Other.Size)
		{
			Other.Data = nullptr;
			Other.Size = 0;
		}

		FORCEINLINE TStringView(const ElementType* InStr) noexcept
			: Data(InStr)
			, Size(strlen(InStr))
		{
		}

		FORCEINLINE TStringView(const ElementType* InStr, const size_t InSize) noexcept
			: Data(InStr)
			, Size(InSize)
		{
		}

		template<IsCharRange T>
		FORCEINLINE TStringView(T&& Range)
			: TStringView(Bloodshot::GetData(Range), Bloodshot::GetSize(std::forward<T>(Range)))
		{
		}

		FORCEINLINE ~TStringView() = default;

		NODISCARD FORCEINLINE const ElementType& operator[](const size_t Index) const noexcept
		{
			return Data[Index];
		}

		FORCEINLINE TStringView& operator=(const TStringView& Other) noexcept
		{
			Data = Other.Data;
			Size = Other.Size;
			return *this;
		}

		FORCEINLINE TStringView& operator=(TStringView&& Other) noexcept
		{
			Data = Other.Data;
			Size = Other.Size;
			Other.Data = nullptr;
			Other.Size = 0;
			return *this;
		}

		FORCEINLINE bool operator==(const TStringView& Other) const noexcept
		{
			return strcmp(Data, Other.Data) == 0;
		}

		FORCEINLINE bool operator==(const ElementType* Str) const noexcept
		{
			return strcmp(Data, Str) == 0;
		}

		NODISCARD FORCEINLINE const ElementType* GetData() const noexcept
		{
			return Data;
		}

		NODISCARD FORCEINLINE size_t GetSize() const noexcept
		{
			return Size;
		}

		NODISCARD FORCEINLINE bool IsEmpty() const noexcept
		{
			return !Size;
		}

		NODISCARD FORCEINLINE TStringView SubStr(const size_t Index, const size_t Count) const noexcept
		{
			return TStringView(Data + Index, Count);
		}

		NODISCARD FORCEINLINE size_t Find(const ElementType* const SubStr) const noexcept
		{
			const size_t SubStrSize = strlen(SubStr);
			size_t SameCharCount = 0;
			for (size_t i = 0; i < SubStrSize; ++i)
			{
				if (Data[i] == SubStr[i])
				{
					++SameCharCount;
				}

				if (SameCharCount == SubStrSize)
				{
					return i - SubStrSize;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool Find(const ElementType* const SubStr, size_t& OutIndex) const noexcept
		{
			OutIndex = Find(SubStr);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE size_t Find(TStringView Other) const noexcept
		{
			return Find(Other.Data);
		}

		NODISCARD FORCEINLINE bool Find(TStringView Other, size_t& OutIndex) const noexcept
		{
			return Find(Other.Data, OutIndex);
		}

		NODISCARD FORCEINLINE size_t FindChar(const ElementType Char) const noexcept
		{
			for (size_t i = 0; i < Size; ++i)
			{
				if (Data[i] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool FindChar(const ElementType Char, size_t& OutIndex) const noexcept
		{
			OutIndex = FindChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE size_t FindLastChar(const ElementType Char) const noexcept
		{
			for (size_t i = Size; i > 0; --i)
			{
				if (Data[i - 1] == Char)
				{
					return i;
				}
			}

			return IReservedValues::NoneIndex;
		}

		NODISCARD FORCEINLINE bool FindLastChar(const ElementType Char, size_t& OutIndex) const noexcept
		{
			OutIndex = FindLastChar(Char);
			return OutIndex != IReservedValues::NoneIndex;
		}

	private:
		const ElementType* Data = nullptr;
		size_t Size = 0;
	};

	template<typename CharType>
	struct TIsZeroConstructible<TStringView<CharType>>
	{
		static constexpr bool Value = true;
	};
}

namespace std
{
	template<typename InCharType>
	struct std::hash<Bloodshot::TStringView<InCharType>>
	{
		size_t operator()(const Bloodshot::TStringView<InCharType>& Str) const noexcept
		{
			return std::_Hash_array_representation(Str.GetData(), Str.GetSize());
		}
	};
}
