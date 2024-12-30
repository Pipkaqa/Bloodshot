#include "Keywords.h"

namespace Bloodshot::HeaderTool::Private
{
	bool IsOperator(std::string_view Char)
	{
		return GOperators.contains(Char);
	}

	bool IsSeparator(std::string_view Char)
	{
		return GSeparators.contains(Char);
	}

	bool IsOperator(const char Char)
	{
		return GOperators.contains(Char);
	}

	bool IsSeparator(const char Char)
	{
		return GSeparators.contains(Char);
	}

	bool IsKeyword(std::string_view Word)
	{
		return std::find(GKeywordArr.begin(), GKeywordArr.end(), Word) != GKeywordArr.end() ? true : false;
	}
}
