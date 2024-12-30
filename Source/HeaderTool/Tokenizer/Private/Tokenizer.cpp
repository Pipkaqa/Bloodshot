#include "Keywords.h"
#include "Tokenizer.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace Bloodshot::HeaderTool::Private
{
	std::vector<FToken> FTokenizer::Tokenize(std::string_view SourceCode, const ETokenizerFlags Flags)
	{
		this->SourceCode = SourceCode;

		std::vector<FToken> Result;

		while (Position < SourceCode.size())
		{
			const char CurrentChar = SourceCode[Position];

			if (std::isspace(CurrentChar))
			{
				Position++;
				continue;
			}

			FToken Token;

			if (CurrentChar == ':' && Peek() == ':')
			{
				Token = Tokenize(ETokenType::ScopeResolutionOperator);
			}
			else if (std::isalpha(CurrentChar) || CurrentChar == '_')
			{
				Token = Tokenize((ETokenType)((uint8_t)ETokenType::Identifier | (uint8_t)ETokenType::Keyword));
			}
			else if (std::isdigit(CurrentChar))
			{
				Token = Tokenize(ETokenType::NumberLiteral);
			}
			else if (CurrentChar == '"')
			{
				Token = Tokenize(ETokenType::StringLiteral);
			}
			else if (CurrentChar == '/' && Peek() == '/')
			{
				Token = Tokenize(ETokenType::Comment);
			}
			else if (Private::IsOperator(CurrentChar))
			{
				Token = Tokenize(ETokenType::Operator);
			}
			else if (Private::IsSeparator(CurrentChar))
			{
				Token = Tokenize(ETokenType::Separator);
			}
			else
			{
				Token = Tokenize(ETokenType::Unknown);
			}

			Result.emplace_back(std::move(Token));
		}

		return Result;
	}

	FToken FTokenizer::Tokenize(const ETokenType Type)
	{
		std::string Token;

		switch (Type)
		{
			case (ETokenType)((uint8_t)ETokenType::Keyword | (uint8_t)ETokenType::Identifier) :
			{
				while (Position < SourceCode.size() && (std::isalnum(SourceCode[Position]) || SourceCode[Position] == '_'))
				{
					Token += SourceCode[Position++];
				}

				ETokenType Type = ETokenType::Identifier;

				if (Private::IsKeyword(Token))
				{
					Type = ETokenType::Keyword;
				}

				return {Type, Token};
			}
			case ETokenType::NumberLiteral:
			{
				while (Position < SourceCode.size() && std::isdigit(SourceCode[Position]))
				{
					Token += SourceCode[Position++];
				}

				break;
			}
			case ETokenType::StringLiteral:
			{
				Token += SourceCode[Position++];

				const size_t SourceCodeSize = SourceCode.size();

				while (Position < SourceCodeSize && SourceCode[Position] != '"')
				{
					Token += SourceCode[Position++];
				}

				if (Position < SourceCodeSize)
				{
					Token += SourceCode[Position++];
				}

				break;
			}
			case ETokenType::Operator:
			{
				Token = std::string(1, SourceCode[Position++]);
				break;
			}
			case ETokenType::ScopeResolutionOperator:
			{
				Position += 2;
				Token = "::";
				break;
			}
			case ETokenType::Separator:
			{
				Token = std::string(1, SourceCode[Position++]);
				break;
			}
			case ETokenType::Comment:
			{
				while (Position < SourceCode.size() && SourceCode[Position] != '\n')
				{
					Token += SourceCode[Position++];
				}

				break;
			}
			case ETokenType::Unknown:
			{
				Token = std::string(1, SourceCode[Position++]);
				break;
			}
		}

		return {Type, Token};
	}

	char FTokenizer::Peek() const
	{
		return (Position + 1 < SourceCode.size()) ? SourceCode[Position + 1] : '\0';
	}
}
