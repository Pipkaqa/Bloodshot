#pragma once

#include "Platform/Platform.h"
#include "Templates/Containers/String.h"
#include "Templates/Containers/Vector.h"

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

	NODISCARD TVector<FString> Split(FString& OutString, const FString& Separator);
}
