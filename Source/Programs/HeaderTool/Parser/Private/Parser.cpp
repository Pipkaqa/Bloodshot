#include "Parser.h"
#include "Keywords.h"

#include <format>

namespace Bloodshot::HeaderTool::Private
{
	std::vector<FClassInfo> FParser::Parse(const std::vector<FToken>& Tokens)
	{
		this->Tokens = Tokens;
		ClassInfos.clear();
		TokenIndex = 0;
		TempClassInfo = FClassInfo();

		while (TokenIndex < Tokens.size())
		{
			if (GetValue() == "BSCLASS" && Tokens[TokenIndex - 1].Value != "define")
			{
				Consume();
				ParseClass();
				continue;
			}

			Consume();
		}

		return ClassInfos;
	}

	const FToken& FParser::Get()
	{
		return Tokens.at(TokenIndex);
	}

	const std::string& FParser::GetValue()
	{
		return Tokens.at(TokenIndex).Value;
	}

	ETokenType FParser::GetType()
	{
		return Tokens.at(TokenIndex).Type;
	}

	const FToken& FParser::Consume()
	{
		return Tokens.at(TokenIndex++);
	}

	const std::string& FParser::ConsumeValue()
	{
		return Tokens.at(TokenIndex++).Value;
	}

	ETokenType FParser::ConsumeType()
	{
		return Tokens.at(TokenIndex++).Type;
	}

	void FParser::Require(std::string_view RequiredToken)
	{
		const std::string& RealToken = ConsumeValue();
		if (RealToken != RequiredToken)
		{
			printf(std::format("Required: {}, but found: ", RequiredToken, RealToken).c_str());
			std::terminate();
		}
	}

	void FParser::Skip(std::string_view TokenToSkip)
	{
		while (GetValue() == TokenToSkip)
		{
			Consume();
		}
	}

	void FParser::ParseType(std::string& OutTypeStr, std::vector<FToken>& OutTokens)
	{
		size_t TemplateBraceCount = 0;

		bool bTemplateSpecializationStarted = false;
		bool bTemplateSpecializationEnded = false;
		bool bTypeCompleted = false;

		std::vector<std::string> ParsedTokens;

		while (OutTokens.size() > 0)
		{
			const FToken& Token = OutTokens.back();
			const std::string& TokenValue = Token.Value;

			auto ExtractToken = [&OutTokens, &ParsedTokens]() -> void
				{
					ParsedTokens.push_back(OutTokens.back().Value);
					OutTokens.pop_back();
				};

			if (TokenValue == ">")
			{
				++TemplateBraceCount;
				bTemplateSpecializationStarted = true;
			}
			else if (TokenValue == "<")
			{
				--TemplateBraceCount;
				if (!TemplateBraceCount) bTemplateSpecializationEnded = true;
			}
			else if (TokenValue == "::")
			{
				ExtractToken();
			}
			else if (!bTemplateSpecializationStarted || bTemplateSpecializationStarted && bTemplateSpecializationEnded)
			{
				if (Token.Type == ETokenType::Identifier)
				{
					if (!bTypeCompleted)
					{
						bTypeCompleted = true;
					}
					else
					{
						OutTokens.pop_back();
						continue;
					}
				}
			}

			ExtractToken();
		}

		std::reverse(ParsedTokens.begin(), ParsedTokens.end());

		for (const std::string& Token : ParsedTokens)
		{
			OutTypeStr += Token;

			if (Token == "const" || Token == "volatile" || Token == "constexpr" || Token == "consteval")
			{
				OutTypeStr += " ";
			}
		}
	}

	void FParser::ParseClassMacroArgs()
	{
		Require("(");
		while (ConsumeValue() != ")") {}
	}

	void FParser::ParseFunctionMacroArgs(FFunctionInfo& OutFunctionInfo)
	{
		Require("(");
		while (ConsumeValue() != ")") {}
	}

	void FParser::ParsePropertyMacroArgs(FPropertyInfo& OutPropertyInfo)
	{
		Require("(");

		while (GetValue() != ")")
		{
			const std::string& Value = ConsumeValue();

			if (Value == "Serialized")
			{
				OutPropertyInfo.bSerialized = true;
			}
			else if (Value == "Replicated")
			{
				OutPropertyInfo.bReplicated = true;
			}
		}

		Consume();
	}

	void FParser::ParseClassDeclaration()
	{
		TempClassInfo.Name = ConsumeValue();

		while (GetValue() != "{")
		{
			ETokenType Type = GetType();
			const std::string& Value = ConsumeValue();

			if (Value == "final")
			{
				TempClassInfo.bFinal = true;
			}
			else if (Value == "abstract")
			{
				TempClassInfo.bAbstract = true;
			}
			else if (Value == ":")
			{
				TempClassInfo.bDerived = true;
			}
			else if (Type == ETokenType::Identifier)
			{
				TempClassInfo.BaseClassNames.push_back(Value);
			}
		}

		Consume();
	}

	void FParser::ParseClassDefinition()
	{
		size_t BraceCount = 1;

		while (BraceCount > 0)
		{
			const std::string& Value = GetValue();

			if (Value == "{")
			{
				++BraceCount;
			}
			else if (Value == "}")
			{
				--BraceCount;
			}
			else if (Value == "BSFUNCTION")
			{
				Consume();
				ParseFunction();
				continue;
			}
			else if (Value == "BSPROPERTY")
			{
				Consume();
				ParseProperty();
			}

			Consume();
		}
	}

	void FParser::ParseClass()
	{
		ParseClassMacroArgs();
		Skip(";");
		Require("class");
		ParseClassDeclaration();
		ParseClassDefinition();

		ClassInfos.emplace_back(std::move(TempClassInfo));
	}

	void FParser::ParseFunctionParameters(FFunctionInfo& OutFunctionInfo)
	{
		size_t BraceCount = 1;

		std::vector<FToken> ParametersTokens;

		while (BraceCount > 0)
		{
			const FToken& Token = Consume();
			const std::string& Value = Token.Value;

			if (Value == "(")
			{
				++BraceCount;
				ParametersTokens.push_back(Token);
			}
			else if (Value == ")")
			{
				--BraceCount;
				if (BraceCount) ParametersTokens.push_back(Token);
			}
			else
			{
				ParametersTokens.push_back(Token);
			}
		}

		if (!ParametersTokens.size()) return;

		std::vector<FToken> ParameterTokens;

		auto ExtractVariableWithoutInitializer = [&OutFunctionInfo, &ParameterTokens, this]()
			{
				std::reverse(ParameterTokens.begin(), ParameterTokens.end());

				std::vector<FToken>::iterator EraseStartIt = ParameterTokens.end();

				for (std::vector<FToken>::iterator It = ParameterTokens.begin(); It != ParameterTokens.end(); ++It)
				{
					const std::string& TokenValue = It->Value;

					if (TokenValue == "=" || TokenValue == "{" || TokenValue == "(")
					{
						EraseStartIt = It;
					}
				}

				ParameterTokens.erase(EraseStartIt, ParameterTokens.end());

				FParameterInfo ParameterInfo;

				ParameterInfo.Name = ParameterTokens.back().Value;
				ParameterTokens.pop_back();

				ParseType(ParameterInfo.Type, ParameterTokens);

				OutFunctionInfo.Parameters.emplace_back(std::move(ParameterInfo));
				ParameterTokens.clear();
			};

		while (ParametersTokens.size() > 0)
		{
			const FToken& Token = ParametersTokens.back();
			const std::string& TokenValue = Token.Value;

			if (TokenValue == ",")
			{
				ExtractVariableWithoutInitializer();
			}
			else
			{
				ParameterTokens.emplace_back(Token);
			}

			ParametersTokens.pop_back();
		}

		if (ParameterTokens.size())
		{
			ExtractVariableWithoutInitializer();
		}
	}

	void FParser::ParseFunction()
	{
		FFunctionInfo FunctionInfo;

		ParseFunctionMacroArgs(FunctionInfo);
		Skip(";");

		std::vector<FToken> TempTokens;

		while (GetValue() != "(")
		{
			TempTokens.push_back(Consume());
		}

		FunctionInfo.Name = TempTokens.back().Value;
		TempTokens.pop_back();

		if (std::find_if(TempTokens.begin(), TempTokens.end(),
			[](const FToken& Token)
			{
				return Token.Value == "static";
			}) != TempTokens.end())
		{
			FunctionInfo.bStatic = true;
		}

		ParseType(FunctionInfo.ReturnType, TempTokens);

		Consume();

		ParseFunctionParameters(FunctionInfo);

		TempClassInfo.Functions.emplace_back(std::move(FunctionInfo));
	}

	void FParser::ParseProperty()
	{
		FPropertyInfo PropertyInfo;

		ParsePropertyMacroArgs(PropertyInfo);
		Skip(";");

		std::vector<FToken> TempTokens;

		while (GetValue() != ";" && GetValue() != "{" && GetValue() != "=")
		{
			TempTokens.push_back(Consume());
		}

		if (std::find_if(TempTokens.begin(), TempTokens.end(),
			[](const FToken& Token)
			{
				return Token.Value == "static";
			}) != TempTokens.end())
		{
			PropertyInfo.bStatic = true;
		}

		PropertyInfo.Name = TempTokens.back().Value;
		TempTokens.pop_back();

		ParseType(PropertyInfo.Type, TempTokens);

		TempClassInfo.Properties.emplace_back(std::move(PropertyInfo));
	}
}
