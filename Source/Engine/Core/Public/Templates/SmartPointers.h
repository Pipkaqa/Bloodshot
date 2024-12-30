#pragma once

#include "Platform.h"

#include <functional>
#include <memory>

namespace Bloodshot
{
	template<typename ElementType>
	using TSharedPtr = std::shared_ptr<ElementType>;

	template<typename ElementType, typename DeleterType = std::default_delete<ElementType>>
	using TUniquePtr = std::unique_ptr<ElementType, DeleterType>;

	template<typename ElementType, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TSharedPtr<ElementType> MakeShared(ArgTypes&&... Args)
	{
		return std::make_shared<ElementType>(std::forward<ArgTypes>(Args)...);
	}

	template<typename ElementType, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TSharedPtr<ElementType> MakeShared(ElementType* const Src)
	{
		return std::shared_ptr<ElementType>(Src);
	}

	template<typename ElementType, typename... ArgTypes>
	NODISCARD FORCEINLINE constexpr TUniquePtr<ElementType, std::default_delete<ElementType>> MakeUnique(ArgTypes&&... Args)
	{
		return std::make_unique<ElementType>(std::forward<ArgTypes>(Args)...);
	}

	template<typename ElementType, typename Deleter = std::default_delete<ElementType>>
	NODISCARD FORCEINLINE constexpr TUniquePtr<ElementType, Deleter> MakeUnique(ElementType* const Src)
	{
		return std::unique_ptr<ElementType, Deleter>(Src);
	}

	template<typename ElementType>
	using TReference = ElementType*;
}
