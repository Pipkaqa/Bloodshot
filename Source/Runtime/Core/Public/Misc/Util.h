#pragma once

#include "Containers/String.h"
#include "Containers/Array.h"
#include "Platform/Platform.h"

#include <functional>

namespace Bloodshot
{
	template <typename ReturnType, typename ClassType, typename... ArgTypes>
	NODISCARD static std::function<ReturnType(ClassType*, ArgTypes...)> ProxyCall(ReturnType(ClassType::* Func)(ArgTypes...))
	{
		return std::function<ReturnType(ClassType*, ArgTypes...)>(Func);
	}

	template <typename ReturnType, typename ClassType, typename... ArgTypes>
	NODISCARD static std::function<ReturnType(ArgTypes...)> Bind(ReturnType(ClassType::* Func)(ArgTypes...), ClassType* Ptr)
	{
		return std::bind(ProxyCall(Func), Ptr);
	}

	NODISCARD TArray<FString> Split(FString& OutString, const FString& Separator);
}
