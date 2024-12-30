#pragma once

#include "Platform.h"

#include <functional>
#include <string>
#include <string_view>
#include <vector>

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

	NODISCARD std::vector<std::string> Split(std::string_view Line, std::string_view Delimiter);
}
