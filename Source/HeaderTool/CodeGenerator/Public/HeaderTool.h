#pragma once

#include "Tokenizer.h"

#include <filesystem>
#include <fstream>
#include <set>
#include <string_view>
#include <vector>

namespace Bloodshot::HeaderTool
{
	namespace Private
	{
		struct FToken;

		struct FPropertyInfo final
		{
			std::string Type;
			std::string Name;

			bool bIsStatic = false;
			bool bSerialized = false;
			bool bReplicated = false;
		};

		struct FParameterInfo final
		{
			std::string Type;
			std::string Name;
		};

		struct FFunctionInfo final
		{
			std::string ReturnType;
			std::string Name;
			std::vector<FParameterInfo> Parameters;

			bool bStatic = false;
			bool bConst = false;
			bool bNoexcept = false;
		};

		struct FClassInfo final
		{
			std::string Name;
			std::vector<std::string> BaseClassNames;
			std::vector<FPropertyInfo> Properties;
			std::vector<FFunctionInfo> Functions;

			bool bAbstract = false;
			bool bFinal = false;
			bool bDerived = false;
		};
	}

	class FHeaderTool final
	{
	public:
		void ParseHeaderFile(const std::filesystem::path& Path);

	private:
		Private::FTokenizer Tokenizer;

		std::vector<Private::FToken> Tokens;
		size_t TokenIndex = 0;

		std::set<std::string> UserDataTypeTokenSet;
		std::vector<Private::FClassInfo> ClassInfoVec;

		Private::FClassInfo ClassInfo;

		void ParseClassMacroArgs();
		void ParseFunctionMacroArgs(Private::FFunctionInfo& OutFunctionInfo);
		void ParsePropertyMacroArgs(Private::FPropertyInfo& OutPropertyInfo);

		void ParseClassDeclaration();
		void ParseClassDefinition();
		void ParseClass();

		void ParseFunctionParameters(Private::FFunctionInfo& OutFunctionInfo);
		void ParseFunction();
		void ParseProperty();
	};
}
