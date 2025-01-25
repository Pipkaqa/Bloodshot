#include "Tokenizer.h"
#include "Keywords.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace Bloodshot::HeaderTool
{
	std::vector<FToken> FTokenizer::Tokenize(std::string_view SourceCode)
	{
		Clear();
		this->SourceCode = SourceCode;

		std::vector<FToken> Result;

		while (Position < SourceCode.size())
		{
			const char CurrentChar = Get();

			if (std::isspace(CurrentChar))
			{
				Consume();
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
			else if (IsOperator(CurrentChar))
			{
				Token = Tokenize(ETokenType::Operator);
			}
			else if (IsSeparator(CurrentChar))
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

	void FTokenizer::Clear()
	{
		SourceCode.clear();
		Position = 0;
	}

	char FTokenizer::Get() const
	{
		return SourceCode.at(Position);
	}

	char FTokenizer::Consume()
	{
		return SourceCode.at(Position++);
	}

	char FTokenizer::Peek() const
	{
		return (Position + 1 < SourceCode.size()) ? SourceCode.at(Position + 1) : '\0';
	}

	FToken FTokenizer::Tokenize(const ETokenType Type)
	{
		const size_t SourceCodeSize = SourceCode.size();

		FToken Token;
		Token.Type = Type;

		switch (Type)
		{
			case (ETokenType)((uint8_t)ETokenType::Keyword | (uint8_t)ETokenType::Identifier) :
			{
				while (Position < SourceCodeSize && (std::isalnum(Get()) || Get() == '_'))
				{
					Token.Value += Consume();
				}

				Token.Type = IsKeyword(Token.Value) ? ETokenType::Keyword : ETokenType::Identifier;
				break;
			}
			case ETokenType::NumberLiteral:
			{
				while (Position < SourceCodeSize && std::isdigit(Get()))
				{
					Token.Value += Consume();
				}

				break;
			}
			case ETokenType::StringLiteral:
			{
				Token.Value += Consume();

				while (Position < SourceCodeSize && Get() != '"')
				{
					Token.Value += Consume();
				}

				if (Position < SourceCodeSize)
				{
					Token.Value += Consume();
				}

				break;
			}
			case ETokenType::Operator:
			{
				Token.Value = Consume();
				break;
			}
			case ETokenType::ScopeResolutionOperator:
			{
				Consume();
				Consume();
				Token.Value = "::";
				break;
			}
			case ETokenType::Separator:
			{
				Token.Value = Consume();
				break;
			}
			case ETokenType::Comment:
			{
				while (Position < SourceCodeSize && Get() != '\n')
				{
					Token.Value += Consume();
				}

				break;
			}
			case ETokenType::Unknown:
			{
				Token.Value = Consume();
				break;
			}
		}

		return Token;
	}
}
