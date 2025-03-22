#pragma once

#include "Containers/StringFwd.h"

#include <functional>

namespace Bloodshot
{
	void ParseTokens(FStringView InView, FStringView InDelimiter, const std::function<void(FStringView)>& InVisitor);
}
