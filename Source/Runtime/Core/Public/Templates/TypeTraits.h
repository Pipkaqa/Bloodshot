#pragma once

#include <concepts>
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
	};

	template<typename T>
	struct TFalseType : std::false_type {};

	namespace Private
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
				if constexpr (Private::THasMemberFieldValue<T>::Value)
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
	struct TAnd<Type, Types...> : TAndValue<Private::TGetValueField<Type>::GetValue(), Types...> {};

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
	struct TOr<Type, Types...> : TOrValue<Private::TGetValueField<Type>::GetValue(), Types...> {};

	template <>
	struct TOr<>
	{
		static constexpr bool Value = false;
	};

	template <typename Type>
	struct TNot
	{
		static constexpr bool Value = !Private::TGetValueField<Type>::GetValue();
	};

	template<typename T>
	struct TIsZeroConstructible
	{
		static constexpr bool Value = TOr<std::is_arithmetic<T>, std::is_enum<T>, std::is_pointer<T>>::Value;
	};

	template<typename T>
	constexpr bool TIsZeroConstructibleValue = TIsZeroConstructible<T>::Value;
}
