#pragma once

#include "Platform/Platform.h"

#include <cstddef>

namespace Bloodshot
{
	namespace Private::TypeHash
	{
#ifdef BS_WIN64
		inline constexpr uint64_t Fnv1aOffsetBasis = 14695981039346656037ull;
		inline constexpr uint64_t Fnv1aPrime = 1099511628211ull;
#else
		inline constexpr uint64_t Fnv1aOffsetBasis = 2166136261u;
		inline constexpr uint64_t Fnv1aPrime = 16777619u;
#endif
		NODISCARD constexpr FORCEINLINE uint64_t GetFnv1aHash(const uint8_t* const InBytes, const size_t InSize)
		{
			uint64_t Hash = Fnv1aOffsetBasis;
			for (uint64_t i = 0; i < InSize; ++i)
			{
				Hash ^= InBytes[i];
				Hash *= Fnv1aPrime;
			}
			return Hash;
		}

		template<typename T>
		NODISCARD FORCEINLINE constexpr uint64_t GetFnv1aHashOfArrayRepresentation(const T* const InData, const size_t InSize)
		{
			return GetFnv1aHash(reinterpret_cast<const uint8_t*>(InData), InSize);
		}
	}

	template<typename T>
		requires std::is_scalar_v<T>
	NODISCARD FORCEINLINE uint64_t GetTypeHash(const T InValue)
	{
		if constexpr (std::is_arithmetic_v<T>)
		{
			return Private::TypeHash::GetFnv1aHash(reinterpret_cast<const uint8_t*>(&InValue), sizeof(T));
		}
		else if constexpr (std::is_enum_v<T>)
		{
			return Private::TypeHash::GetFnv1aHash(reinterpret_cast<const uint8_t*>(&(__underlying_type(T))InValue), sizeof(T));
		}
		else if constexpr (std::is_pointer_v<T>)
		{
			return Private::TypeHash::GetFnv1aHash(&reinterpret_cast<const uint8_t&>(InValue), sizeof(T));
		}
		else
		{
			static_assert(sizeof(T) == 0, "Unsupported scalar type");
			return 0;
		}
	}
}
