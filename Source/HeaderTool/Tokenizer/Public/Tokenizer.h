#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
		ETokenType Type;
		std::string Value;
	};

	// BSTODO: Add more flags
	enum class ETokenizerFlags : uint8_t
	{
		None = 0,
		IgnoreFunctionBodies
	};

	class FTokenizer final
	{
	public:
		std::vector<FToken> Tokenize(std::string_view SourceCode, const ETokenizerFlags Flags = ETokenizerFlags::None);

	private:
		std::string SourceCode;
		size_t Position = 0;

		FToken Tokenize(const ETokenType Type);
		char Peek() const;
	};
}
