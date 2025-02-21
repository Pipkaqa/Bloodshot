#pragma once

#include <initializer_list>
#include <type_traits>

namespace Bloodshot
{
	namespace Private::ElementType
	{
		template <typename T, typename = void>
		struct TImpl
		{
		};

		template <typename T>
		struct TImpl<T, std::void_t<typename T::ElementType>>
		{
			using Type = typename T::ElementType;
		};
	}

	template <typename T>
	struct TElementType : Private::ElementType::TImpl<T>
	{
	};

	template <typename T>
	struct TElementType<T&> : TElementType<T> {};

	template <typename T>
	struct TElementType<T&&> : TElementType<T> {};

	template <typename T>
	struct TElementType<const T> : TElementType<T> {};

	template <typename T>
	struct TElementType<volatile T> : TElementType<T> {};

	template <typename T>
	struct TElementType<const volatile T> : TElementType<T> {};

	template <typename T, size_t Size>
	struct TElementType<T[Size]>
	{
		using Type = T;
	};

	template <typename T, size_t Size>
	struct TElementType<const T[Size]>
	{
		using Type = T;
	};

	template <typename T, size_t Size>
	struct TElementType<volatile T[Size]>
	{
		using Type = T;
	};

	template <typename T, size_t Size>
	struct TElementType<const volatile T[Size]>
	{
		using Type = T;
	};

	template <typename T>
	struct TElementType<std::initializer_list<T>>
	{
		using Type = std::remove_cv_t<T>;
	};

	template <typename T>
	using TElementType_T = typename TElementType<T>::Type;
}
