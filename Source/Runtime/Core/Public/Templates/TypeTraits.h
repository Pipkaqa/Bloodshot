#pragma once

#include <concepts>
#include <initializer_list>
#include <type_traits>

#define GENERATE_MEMBER_FUNCTION_CHECK(MemberName, Result, ConstModifier, ...) \
template <typename T> \
class THasMemberFunction##MemberName \
{ \
private: \
	template <typename U, Result(U::*)(__VA_ARGS__) ConstModifier> \
	struct Check; \
	template <typename U> \
	static char Test(Check<U, &U::MemberName>*); \
	template <typename U> \
	static int Test(...); \
public: \
    static constexpr bool Value = sizeof(Test<T>(nullptr)) == sizeof(char); \
}; \
template<typename T> \
constexpr bool THasMemberFunction##MemberName##Value = THasMemberFunction##MemberName##<T>::Value

namespace Bloodshot
{
	template<typename T>
	concept IsIntegral = std::is_integral_v<T>;

	template<typename T>
	concept IsFloatingPoint = std::is_floating_point_v<T>;

	template <typename T>
	concept IsContainer = requires(T Container)
	{
		typename T::iterator;
		{ Container.begin() } -> std::same_as<typename T::iterator>;
		{ Container.end() } -> std::same_as<typename T::iterator>;
	} || requires(T Container)
	{
		typename T::FRangeForIterator;
		{ Container.begin() } -> std::same_as<typename T::FRangeForIterator>;
		{ Container.end() } -> std::same_as<typename T::FRangeForIterator>;
	};

	template<typename T>
	struct TFalseType : std::false_type {};

	template<typename T>
	constexpr inline bool TFalseType_V = TFalseType<T>::value;

	namespace Private::TypeTraits
	{
		template <typename T>
		struct THasMemberFieldValue
		{
			using TrueType = char;
			using FalseType = int;

			template <typename U>
			static TrueType HasValueImpl(decltype(&U::Value));

			template <typename U>
			static FalseType HasValueImpl(...);

			static constexpr bool Value = sizeof(HasValueImpl<T>(0)) == sizeof(TrueType);
		};

		template<typename T>
		struct TGetValueField
		{
			static consteval bool GetValue()
			{
				if constexpr (Private::TypeTraits::THasMemberFieldValue<T>::Value)
				{
					return T::Value;
				}
				else
				{
					return T::value;
				}
			}
		};
	}

	template <typename... Types>
	struct TAnd;

	template <bool Val, typename... Types>
	struct TAndValue
	{
		static constexpr bool Value = TAnd<Types...>::Value;
	};

	template <typename... Types>
	struct TAndValue<false, Types...>
	{
		static constexpr bool Value = false;
	};

	template <typename Type, typename... Types>
	struct TAnd<Type, Types...> : TAndValue<Private::TypeTraits::TGetValueField<Type>::GetValue(), Types...> {};

	template <>
	struct TAnd<>
	{
		static constexpr bool Value = true;
	};

	template <typename... Types>
	struct TOr;

	template <bool Val, typename... Types>
	struct TOrValue
	{
		static constexpr bool Value = TOr<Types...>::Value;
	};

	template <typename... Types>
	struct TOrValue<true, Types...>
	{
		static constexpr bool Value = true;
	};

	template <typename Type, typename... Types>
	struct TOr<Type, Types...> : TOrValue<Private::TypeTraits::TGetValueField<Type>::GetValue(), Types...> {};

	template <>
	struct TOr<>
	{
		static constexpr bool Value = false;
	};

	template <typename Type>
	struct TNot
	{
		static constexpr bool Value = !Private::TypeTraits::TGetValueField<Type>::GetValue();
	};

	template<typename T>
	struct TIsZeroConstructible
	{
		static constexpr bool Value = TOr<std::is_arithmetic<T>, std::is_enum<T>, std::is_pointer<T>>::Value;
	};

	template<typename T>
	constexpr inline bool TIsZeroConstructible_V = TIsZeroConstructible<T>::Value;

	template<typename T>
	struct TCallTraitsBase
	{
	private:
		static constexpr bool PassByValue = TOr<TAndValue<(sizeof(T) <= sizeof(void*)), std::is_trivially_copyable<T>>, std::is_arithmetic<T>>::Value;

		template<typename T, bool PassByValue>
		struct Helper
		{
			using ParamType = const T&;
			using ConstParamType = const T&;
		};

		template<typename T>
		struct Helper<T, true>
		{
			using ParamType = const T;
			using ConstParamType = const T;
		};

		template<typename T>
		struct Helper<T*, true>
		{
			using ParamType = T*;
			using ConstParamType = const T*;
		};

	public:
		using ConstReferenceType = const T&;
		using ParamType = Helper<T, PassByValue>::ParamType;
		using ConstPointerType = Helper<T, PassByValue>::ConstParamType;
	};

	template<typename T>
	struct TCallTraits : public TCallTraitsBase<T> {};

	template<typename T>
	struct TCallTraits<T&>
	{
		using ConstReferenceType = const T&;
		using ParamType = T&;
		using ConstPointerType = T&;
	};

	template <typename T, size_t Size>
	struct TCallTraits<T[Size]>
	{
	private:
		using ArrayType = T[Size];

	public:
		using ConstReferenceType = const ArrayType&;
		using ParamType = const T* const;
		using ConstPointerType = const T* const;
	};

	template <typename T, size_t Size>
	struct TCallTraits<const T[Size]>
	{
	private:
		using ArrayType = const T[Size];

	public:
		using ConstReferenceType = const ArrayType&;
		using ParamType = const T* const;
		using ConstPointerType = const T* const;
	};
}
