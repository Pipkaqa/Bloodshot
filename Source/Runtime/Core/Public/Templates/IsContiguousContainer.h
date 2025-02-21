#pragma once

#include <initializer_list>

namespace Bloodshot
{
	template <typename T>
	struct TIsContiguousContainer
	{
		static constexpr bool Value = true;
	};

	template<typename T>
	constexpr inline bool TIsContiguousContainer_V = TIsContiguousContainer<T>::Value;

	template<typename T>
	concept IsContiguousContainer = TIsContiguousContainer_V<T>;

	template <typename T>
	struct TIsContiguousContainer<T&> : TIsContiguousContainer<T> {};

	template <typename T>
	struct TIsContiguousContainer<T&&> : TIsContiguousContainer<T> {};

	template <typename T>
	struct TIsContiguousContainer<const T> : TIsContiguousContainer<T> {};

	template <typename T>
	struct TIsContiguousContainer<volatile T> : TIsContiguousContainer<T> {};

	template <typename T>
	struct TIsContiguousContainer<const volatile T> : TIsContiguousContainer<T> {};

	template <typename T, size_t Size>
	struct TIsContiguousContainer<T[Size]>
	{
		static constexpr bool Value = true;
	};

	template <typename T, size_t Size>
	struct TIsContiguousContainer<const T[Size]>
	{
		static constexpr bool Value = true;
	};

	template <typename T, size_t Size>
	struct TIsContiguousContainer<volatile T[Size]>
	{
		static constexpr bool Value = true;
	};

	template <typename T, size_t Size>
	struct TIsContiguousContainer<const volatile T[Size]>
	{
		static constexpr bool Value = true;
	};

	template <typename T>
	struct TIsContiguousContainer<T[]>
	{
		static constexpr bool Value = false;
	};

	template <typename T>
	struct TIsContiguousContainer<const T[]>
	{
		static constexpr bool Value = false;
	};

	template <typename T>
	struct TIsContiguousContainer<volatile T[]>
	{
		static constexpr bool Value = false;
	};

	template <typename T>
	struct TIsContiguousContainer<const volatile T[]>
	{
		static constexpr bool Value = false;
	};

	template <typename T>
	struct TIsContiguousContainer<std::initializer_list<T>>
	{
		static constexpr bool Value = true;
	};
}
