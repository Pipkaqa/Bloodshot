#pragma once

#include <string_view>

namespace Bloodshot::HeaderTool::Private
{
	bool IsIdentifier(std::string_view Word);
	bool IsKeyword(std::string_view Word);
	bool IsOperator(const char Char);
	bool IsSeparator(const char Char);
}