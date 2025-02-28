#pragma once

#include "Platform/Platform.h"

#include <string>
#include <vector>

namespace Bloodshot
{
	NODISCARD std::vector<std::string> Split(std::string& String, const std::string& Separator);
}
