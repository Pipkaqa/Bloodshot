#pragma once

#include "Containers/StringFwd.h"

#include <functional>

namespace Bloodshot
{
	void ParseTokens(FStringView InView, FStringView InDelimiter, std::function<void(FStringView)> InVisitor);
}
