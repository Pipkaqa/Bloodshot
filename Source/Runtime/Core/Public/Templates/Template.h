#pragma once

#include "Misc/CoreMisc.h"
#include "Platform/Platform.h"
#include "Templates/IsContiguousContainer.h"

namespace Bloodshot
{
	template<IsContiguousContainer T>
	NODISCARD constexpr auto GetData(T&& Container) -> decltype(Container.GetData())
	{
		return Container.GetData();
	}

	template <typename T, size_t Size>
	NODISCARD constexpr T* GetData(T(&Container)[Size])
	{
		return Container;
	}

	template <typename T, size_t Size>
	NODISCARD constexpr T* GetData(T(&& Container)[Size])
	{
		return Container;
	}

	template <typename T, size_t Size>
	NODISCARD constexpr const T* GetData(const T(&Container)[Size])
	{
		return Container;
	}

	template <typename T, size_t Size>
	NODISCARD constexpr const T* GetData(const T(&& Container)[Size])
	{
		return Container;
	}

	template <typename T>
	NODISCARD constexpr const T* GetData(std::initializer_list<T> InitList)
	{
		return InitList.begin();
	}

	template<IsContiguousContainer T>
	NODISCARD constexpr auto GetSize(T&& Container) -> decltype(Container.GetSize())
	{
		return Container.GetSize();
	}

	template <typename T, size_t Size>
	NODISCARD constexpr size_t GetSize(T(&Container)[Size])
	{
		return Size;
	}

	template <typename T, size_t Size>
	NODISCARD constexpr size_t GetSize(T(&& Container)[Size])
	{
		return Size;
	}

	template <typename T, size_t Size>
	NODISCARD constexpr size_t GetSize(const T(&Container)[Size])
	{
		return Size;
	}

	template <typename T, size_t Size>
	NODISCARD constexpr size_t GetSize(const T(&& Container)[Size])
	{
		return Size;
	}

	template <typename T>
	NODISCARD constexpr size_t GetSize(std::initializer_list<T> InitList)
	{
		return InitList.size();
	}

	template <typename T, typename ArgType>
	NODISCARD FORCEINLINE T StaticCast(ArgType&& Arg)
	{
		return static_cast<T>(Arg);
	}

	template<typename... ElementTypes>
	class TTypeList
	{
	public:
		template<typename ElementType>
		static consteval size_t Occurrences()
		{
			size_t Counter = 0;
			OccurrencesImpl<ElementType, ElementTypes...>(Counter);
			return Counter;
		}

		template<typename ElementType>
		static consteval bool IsUnique()
		{
			constexpr size_t Occurrences = TTypeList::Occurrences<ElementType>();
			static_assert(Occurrences != 0, "FTypeList::IsUnique: No occurrences of searching type");
			return Occurrences == 1;
		}

		template<typename ElementType>
		static consteval size_t GetIndex()
		{
			static_assert(IsUnique<ElementType>(), "TTypeList::GetIndex: Ambiguous call, searching type is not unique");
			constexpr size_t Index = GetIndexImpl<ElementType, ElementTypes...>(0);
			static_assert(Index != IReservedValues::NoneIndex, "TTypeList::GetIndex: Trying to get index of missing type");
			return Index;
		}

	private:
		template<typename ElementType, typename HeadType, typename... TailType>
		static consteval void OccurrencesImpl(size_t& OutCounter)
		{
			if constexpr (sizeof...(TailType) == 0)
			{
				return;
			}
			else if constexpr (sizeof...(TailType) == 1)
			{
				if constexpr (std::is_same_v<ElementType, HeadType>)
				{
					++OutCounter;
				}
				if constexpr (std::is_same_v<ElementType, TailType...>)
				{
					++OutCounter;
				}
				return;
			}
			else
			{
				if constexpr (std::is_same_v<ElementType, HeadType>)
				{
					++OutCounter;
				}
				OccurrencesImpl<ElementType, TailType...>(OutCounter);
			}
		}

		template<typename ElementType, typename HeadType, typename... TailType>
		static consteval size_t GetIndexImpl(size_t Counter)
		{
			if constexpr (sizeof...(TailType) == 0)
			{
				return IReservedValues::NoneIndex;
			}
			else if constexpr (sizeof...(TailType) == 1)
			{
				if constexpr (std::is_same_v<ElementType, HeadType>)
				{
					return Counter;
				}
				++Counter;
				if constexpr (std::is_same_v<ElementType, TailType...>)
				{
					return Counter;
				}
				return IReservedValues::NoneIndex;
			}
			else
			{
				if constexpr (std::is_same_v<ElementType, HeadType>)
				{
					return Counter;
				}
				++Counter;
				return GetIndexImpl<ElementType, TailType...>(Counter);
			}
		}
	};

	template<size_t Size, size_t Alignment>
	struct TAlignedBytes
	{
		alignas(Alignment) uint8_t Bytes[Size];
	};
}
