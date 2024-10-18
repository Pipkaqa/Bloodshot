#pragma once

#include "Platform.h"

#include <memory>

namespace Bloodshot
{
	template<typename T>
	using TSharedPtr = std::shared_ptr<T>;

	template<typename T, typename Deleter = std::default_delete<T>>
	using TUniquePtr = std::unique_ptr<T, Deleter>;

	template<typename T, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TSharedPtr<T> MakeShared(ArgTypes&&... Args)
	{
		return std::make_shared<T>(std::forward<ArgTypes>(Args)...);
	}

	template<typename T, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TSharedPtr<T> MakeShared(T* const Src)
	{
		return std::shared_ptr<T>(Src);
	}

	template<typename T, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TUniquePtr<T, std::default_delete<T>> MakeUnique(ArgTypes&&... Args)
	{
		return std::make_unique<T>(std::forward<ArgTypes>(Args)...);
	}

	template<typename T, typename Deleter = std::default_delete<T>>
	NODISCARD FORCEINLINE constexpr TUniquePtr<T, Deleter> MakeUnique(T* const Src)
	{
		return std::unique_ptr<T, Deleter>(Src);
	}
}
