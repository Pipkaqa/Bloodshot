#pragma once

#include "Token.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Bloodshot::HeaderTool
{
	class FTokenizer final
	{
	public:
		std::vector<FToken> Tokenize(std::string_view SourceCode);
		void Clear();

	private:
		std::string SourceCode;
		size_t Position = 0;

		char Get() const;
		char Consume();

		char Peek() const;

		FToken Tokenize(const ETokenType Type);
	};
}
