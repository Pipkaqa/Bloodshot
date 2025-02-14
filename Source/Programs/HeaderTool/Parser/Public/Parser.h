#pragma once

#include "Token.h"
#include "TypeInfo.h"

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace Bloodshot::HeaderTool
{
	class FParser final
	{
	public:
		std::vector<FClassInfo> Parse(const std::vector<FToken>& Tokens);

	private:
		struct FNamespaceInfo final
		{
			size_t Index;
			std::string Name;
		};

		std::vector<FClassInfo> ClassInfos;
		std::vector<FToken> Tokens;
		size_t TokenIndex = 0;
		FClassInfo TempClassInfo;
		std::list<FNamespaceInfo> NamespaceStack;

		const FToken& Get();
		const std::string& GetValue();
		ETokenType GetType();

		const FToken& Consume();
		const std::string& ConsumeValue();
		ETokenType ConsumeType();

		void Require(std::string_view RequiredToken);
		void Skip(std::string_view TokenToSkip);

		void ParseType(std::string& OutTypeStr, std::vector<FToken>& OutTokens);

		void ParseClassMacroArgs();
		void ParseFunctionMacroArgs(FFunctionInfo& OutFunctionInfo);
		void ParsePropertyMacroArgs(FPropertyInfo& OutPropertyInfo);

		void ParseClassDeclaration();
		void ParseClassDefinition();
		void ParseClass();

		void ParseFunctionParameters(FFunctionInfo& OutFunctionInfo);
		void ParseFunction();
		void ParseProperty();
	};
}
