#pragma once

#include "Memory/Memory.h"
#include "Platform/Platform.h"
#include "Templates/IsContiguousContainer.h"
#include "Templates/TypeTraits.h"

namespace Bloodshot
{
	namespace Private::String
	{
		class FLowLevelString
		{
		public:
			using ElementType = char;

			NODISCARD FORCEINLINE char& operator[](const size_t Index) noexcept
			{
				return Data[Index];
			}

			NODISCARD FORCEINLINE char* GetData() noexcept
			{
				return Data;
			}

			NODISCARD FORCEINLINE size_t GetSize() const noexcept
			{
				return Size;
			}

			FORCEINLINE char* Init(char* const InData, const size_t InSize)
			{
				Data = InData;
				Size = InSize;
			}

			FORCEINLINE char* Allocate(const size_t InSize)
			{
				Size = InSize;
				Data = (char*)FMemory::Malloc(Size);
				return Data;
			}

			FORCEINLINE void Release()
			{
				Bloodshot::FMemory::Free(Data);
				Data = nullptr;
				Size = 0;
			}

			char* Data = nullptr;
			size_t Size = 0;
		};
	}

	template<>
	struct TIsZeroConstructible<Private::String::FLowLevelString>
	{
		static constexpr bool Value = true;
	};

	template<>
	struct TIsContiguousContainer<Private::String::FLowLevelString>
	{
		static constexpr bool Value = true;
	};
}
