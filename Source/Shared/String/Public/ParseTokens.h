#pragma once

#include <functional>
#include <string_view>

namespace Bloodshot::Shared
{
	void ParseTokens(std::string_view InView, std::string_view InDelimiter, std::function<void(std::string_view)> InVisitor);
}
