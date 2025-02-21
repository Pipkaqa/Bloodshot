#pragma once

namespace Bloodshot
{
	template<typename T>
	struct TIsCharType
	{
		static constexpr bool Value = false;
	};

	template<>
	struct TIsCharType<char>
	{
		static constexpr bool Value = false;
	};

	template <typename T>
	struct TIsCharType<const T>
	{
		static constexpr bool Value = TIsCharType<T>::Value;
	};

	template <typename T>
	struct TIsCharType<volatile T>
	{
		static constexpr bool Value = TIsCharType<T>::Value;
	};

	template <typename T>
	struct TIsCharType<const volatile T>
	{
		static constexpr bool Value = TIsCharType<T>::Value;
	};

	template <typename T>
	constexpr inline bool TIsCharType_V = TIsCharType<T>::Value;

	template<typename T>
	concept IsCharType = TIsCharType_V<T>;
}
