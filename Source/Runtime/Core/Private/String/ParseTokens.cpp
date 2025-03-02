#include "String/ParseTokens.h"
#include "Containers/StringView.h"

namespace Bloodshot
{
	void ParseTokens(FStringView InView, FStringView InDelimiter, const std::function<void(FStringView)>& InVisitor)
	{
		const char* ViewData = InView.GetData();
		size_t ViewSize = InView.GetSize();
		const char* DelimiterData = InDelimiter.GetData();
		const size_t DelimiterSize = InDelimiter.GetSize();
		size_t SameCharCount = 0;
		FStringView TokenView;
		for (size_t i = 0; i < ViewSize;)
		{
			if (SameCharCount == DelimiterSize)
			{
				TokenView = FStringView(ViewData, i - DelimiterSize);
				InVisitor(TokenView);
				ViewData += i;
				ViewSize -= i;
				SameCharCount = 0;
				i = 0;
				continue;
			}
			else if (ViewData[i] == DelimiterData[SameCharCount])
			{
				++SameCharCount;
			}
			++i;
		}
		TokenView = FStringView(ViewData, ViewSize);
		InVisitor(TokenView);
	}
}
