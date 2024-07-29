#pragma once

#include "Platform/Platform.h"

#include <functional>
#include <memory>

namespace Bloodshot
{
	template<typename T, typename Deleter = std::default_delete<T>>
	using UniquePointer = std::unique_ptr<T, Deleter>;

	template<typename T, typename ... Args>
	NODISCARD constexpr UniquePointer<T, std::default_delete<T>> CreateUniquePointer(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename Deleter = std::default_delete<T>>
	NODISCARD constexpr UniquePointer<T, Deleter> CreateUniquePointer(T* ptr)
	{
		return std::unique_ptr<T, Deleter>(std::move(ptr));
	}

	template <typename Return, typename Class, typename... Args>
	NODISCARD std::function<Return(Class*, Args...)> ProxyCall(Return(Class::* func)(Args...))
	{
		return std::function<Return(Class*, Args...)>(func);
	}

	template <typename Return, typename Class, typename... Args>
	NODISCARD std::function<Return(Args...)> Bind(Return(Class::* func)(Args...), Class* ptr)
	{
		return std::bind(ProxyCall(func), ptr);
	}

	template<typename To, typename From>
	NODISCARD FORCEINLINE To SafeCast(From from)
	{
		return static_cast<To>(from);
	}

	template<typename To, typename From>
	NODISCARD FORCEINLINE To FastCast(From from)
	{
		return reinterpret_cast<To>(from);
	}
}