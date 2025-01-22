#pragma once

#include "Token.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Bloodshot::HeaderTool::Private
{
	class FTokenizer final
	{
	public:
		std::vector<FToken> Tokenize(std::string_view SourceCode);

	private:
		std::string SourceCode;
		size_t Position{};

		char Get() const;
		char Consume();

		char Peek() const;

		FToken Tokenize(const ETokenType Type);
	};
}
