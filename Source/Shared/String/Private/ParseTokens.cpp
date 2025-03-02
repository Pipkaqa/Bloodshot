#include "ParseTokens.h"

namespace Bloodshot::Shared
{
	void ParseTokens(std::string_view InView, std::string_view InDelimiter, std::function<void(std::string_view)> InVisitor)
	{
		const char* ViewData = InView.data();
		size_t ViewSize = InView.size();
		const char* DelimiterData = InDelimiter.data();
		const size_t DelimiterSize = InDelimiter.size();
		size_t SameCharCount = 0;
		std::string_view TokenView;
		for (size_t i = 0; i < ViewSize;)
		{
			if (SameCharCount == DelimiterSize)
			{
				TokenView = std::string_view(ViewData, i - DelimiterSize);
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
		TokenView = std::string_view(ViewData, ViewSize);
		InVisitor(TokenView);
	}
}
