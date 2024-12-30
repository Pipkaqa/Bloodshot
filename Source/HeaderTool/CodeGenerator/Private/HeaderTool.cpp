#include "HeaderTool.h"
#include "Keywords.h"

#include <exception>
#include <sstream>
#include <string>
#include <type_traits>

namespace
{
	static void ParseType(std::string& OutTypeStr, std::vector<std::string>& Tokens)
	{
		using namespace Bloodshot::HeaderTool::Private;

		size_t TemplateBraceCount = 0;

		bool bIsTemplateSpecializationStarted = false;
		bool bIsTemplateSpecializationEnded = false;
		bool bIsTypeCompleted = false;

		std::vector<std::string> ParsedTokens;

		while (Tokens.size() > 0)
		{
			std::string& Token = Tokens.back();

			auto IsIdentifier = [](const std::string& Token) -> bool
				{
					return !IsKeyword(Token)
						&& !IsOperator(Token)
						&& !IsSeparator(Token);
				};

			auto ExtractToken = [&Token, &Tokens, &ParsedTokens]() -> void
				{
					ParsedTokens.push_back(Tokens.back());
					Tokens.pop_back();
				};

			if (Token == ">")
			{
				++TemplateBraceCount;
				bIsTemplateSpecializationStarted = true;
			}
			else if (Token == "<")
			{
				--TemplateBraceCount;
				if (!TemplateBraceCount) bIsTemplateSpecializationEnded = true;
			}
			else if (Token == "::")
			{
				ExtractToken();
			}
			else if (!bIsTemplateSpecializationStarted || bIsTemplateSpecializationStarted && bIsTemplateSpecializationEnded)
			{
				if (IsIdentifier(Token))
				{
					if (!bIsTypeCompleted)
					{
						bIsTypeCompleted = true;
					}
					else
					{
						Tokens.pop_back();
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
}

namespace Bloodshot::HeaderTool
{
	void FHeaderTool::ParseHeaderFile(const std::filesystem::path& Path)
	{
		std::ifstream InputFileStream(Path.string().c_str());
		if (!InputFileStream.is_open()) terminate();

		std::string SourceCode = (std::stringstream() << InputFileStream.rdbuf()).str();
		Tokens = std::move(Tokenizer.Tokenize(SourceCode));

		while (TokenIndex < Tokens.size())
		{
			if (Tokens[TokenIndex].Value == "BSCLASS" && Tokens[TokenIndex - 1].Value != "define")
			{
				++TokenIndex;
				ParseClass();
				continue;
			}

			++TokenIndex;
		}
	}

	void FHeaderTool::ParseClassMacroArgs()
	{
		if (Tokens[TokenIndex++].Value != "(")
		{
			terminate();
		}

		while (Tokens[TokenIndex].Value != ")")
		{
			++TokenIndex;
		}

		++TokenIndex;
	}

	void FHeaderTool::ParseFunctionMacroArgs(Private::FFunctionInfo& OutFunctionInfo)
	{
		if (Tokens[TokenIndex++].Value != "(")
		{
			terminate();
		}

		while (Tokens[TokenIndex].Value != ")")
		{
			++TokenIndex;
		}

		++TokenIndex;
	}

	void FHeaderTool::ParsePropertyMacroArgs(Private::FPropertyInfo& OutPropertyInfo)
	{
		if (Tokens[TokenIndex++].Value != "(")
		{
			terminate();
		}

		while (Tokens[TokenIndex].Value != ")")
		{
			if (Tokens[TokenIndex].Value == "Serialized")
			{
				OutPropertyInfo.bSerialized = true;
			}
			else if (Tokens[TokenIndex].Value == "Replicated")
			{
				OutPropertyInfo.bReplicated = true;
			}

			++TokenIndex;
		}

		++TokenIndex;
	}

	void FHeaderTool::ParseClassDeclaration()
	{
		ClassInfo.Name = Tokens[TokenIndex++].Value;

		while (Tokens[TokenIndex].Value != "{")
		{
			if (Tokens[TokenIndex].Value == "final")
			{
				ClassInfo.bFinal = true;
			}
			else if (Tokens[TokenIndex].Value == "abstract")
			{
				ClassInfo.bAbstract = true;
			}
			else if (Tokens[TokenIndex].Value == ":")
			{
				ClassInfo.bDerived = true;
			}
			else if (!Private::IsKeyword(Tokens[TokenIndex].Value)
				&& !Private::IsOperator(Tokens[TokenIndex].Value)
				&& !Private::IsSeparator(Tokens[TokenIndex].Value))
			{
				ClassInfo.BaseClassNames.emplace_back(Tokens[TokenIndex].Value);
			}

			++TokenIndex;
		}

		++TokenIndex;
	}

	void FHeaderTool::ParseClassDefinition()
	{
		size_t BraceCount = 1;

		while (BraceCount > 0)
		{
			if (Tokens[TokenIndex].Value == "{")
			{
				++BraceCount;
			}
			else if (Tokens[TokenIndex].Value == "}")
			{
				--BraceCount;
			}
			else if (Tokens[TokenIndex].Value == "BSFUNCTION")
			{
				++TokenIndex;
				ParseFunction();
				continue;
			}
			else if (Tokens[TokenIndex].Value == "BSPROPERTY")
			{
				++TokenIndex;
				ParseProperty();
			}

			++TokenIndex;
		}
	}

	void FHeaderTool::ParseClass()
	{
		ClassInfo = {};

		ParseClassMacroArgs();

		if (Tokens[TokenIndex++].Value != "class")
		{
			terminate();
		}

		ParseClassDeclaration();
		ParseClassDefinition();

		ClassInfoVec.emplace_back(std::move(ClassInfo));
	}

	void FHeaderTool::ParseFunctionParameters(Private::FFunctionInfo& OutFunctionInfo)
	{
		size_t BraceCount = 1;

		std::vector<std::string> ParametersTokens;

		while (BraceCount > 0)
		{
			const std::string& Token = Tokens[TokenIndex++].Value;

			if (Token == "(")
			{
				++BraceCount;
				ParametersTokens.emplace_back(Token);
			}
			else if (Token == ")")
			{
				--BraceCount;
				if (BraceCount) ParametersTokens.emplace_back(Token);
			}
			else
			{
				ParametersTokens.emplace_back(Token);
			}
		}

		if (!ParametersTokens.size()) return;

		std::vector<std::string> ParameterTokens;

		auto ExtractVariableWithoutInitializer = [&OutFunctionInfo, &ParameterTokens]()
			{
				std::reverse(ParameterTokens.begin(), ParameterTokens.end());

				std::vector<std::string>::iterator EraseStartIt = ParameterTokens.end();

				for (std::vector<std::string>::iterator It = ParameterTokens.begin(); It != ParameterTokens.end(); ++It)
				{
					if (*It == "=" || *It == "{" || *It == "(")
					{
						EraseStartIt = It;
					}
				}

				ParameterTokens.erase(EraseStartIt, ParameterTokens.end());

				Private::FParameterInfo ParameterInfo;

				ParameterInfo.Name = ParameterTokens.back();
				ParameterTokens.pop_back();

				ParseType(ParameterInfo.Type, ParameterTokens);

				OutFunctionInfo.Parameters.emplace_back(std::move(ParameterInfo));
				ParameterTokens.clear();
			};

		while (ParametersTokens.size() > 0)
		{
			const std::string& Token = ParametersTokens.back();

			if (Token == ",")
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

	void FHeaderTool::ParseFunction()
	{
		Private::FFunctionInfo FunctionInfo;

		ParseFunctionMacroArgs(FunctionInfo);

		std::vector<std::string> TempTokens;

		while (Tokens[TokenIndex].Value != "(")
		{
			TempTokens.emplace_back(Tokens[TokenIndex++].Value);
		}

		FunctionInfo.Name = TempTokens.back();
		TempTokens.pop_back();

		if (std::find(TempTokens.begin(), TempTokens.end(), "static") != TempTokens.end())
		{
			FunctionInfo.bStatic = true;
		}

		ParseType(FunctionInfo.ReturnType, TempTokens);

		++TokenIndex;

		ParseFunctionParameters(FunctionInfo);

		ClassInfo.Functions.emplace_back(std::move(FunctionInfo));
	}

	void FHeaderTool::ParseProperty()
	{
		Private::FPropertyInfo PropertyInfo;

		ParsePropertyMacroArgs(PropertyInfo);

		std::vector<std::string> TempTokens;

		while (Tokens[TokenIndex].Value != ";" && Tokens[TokenIndex].Value != "{" && Tokens[TokenIndex].Value != "=")
		{
			TempTokens.emplace_back(Tokens[TokenIndex++].Value);
		}

		if (std::find(TempTokens.begin(), TempTokens.end(), "static") != TempTokens.end())
		{
			PropertyInfo.bIsStatic = true;
		}

		PropertyInfo.Name = TempTokens.back();
		TempTokens.pop_back();

		ParseType(PropertyInfo.Type, TempTokens);

		ClassInfo.Properties.emplace_back(std::move(PropertyInfo));
	}
}
