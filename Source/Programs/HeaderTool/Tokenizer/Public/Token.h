#pragma once

#include <cstdint>
#include <string>

namespace Bloodshot::HeaderTool::Private
{
	enum class ETokenType : uint8_t
	{
		Keyword = 0,
		Identifier,
		NumberLiteral,
		StringLiteral,
		Operator,
		ScopeResolutionOperator,
		Separator,
		Comment,
		Unknown
	};

	struct FToken final
	{
		ETokenType Type = ETokenType::Unknown;
		std::string Value;
	};
}
