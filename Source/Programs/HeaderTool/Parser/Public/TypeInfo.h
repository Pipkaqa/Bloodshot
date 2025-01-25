#pragma once

#include <string>
#include <vector>

namespace Bloodshot::HeaderTool
{
	struct FPropertyInfo final
	{
		std::string Type;
		std::string Name;

		bool bStatic = false;
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