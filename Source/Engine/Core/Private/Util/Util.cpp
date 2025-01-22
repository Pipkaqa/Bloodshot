#include "Util/Util.h"

namespace Bloodshot
{
	TVector<FString> Split(FString& OutString, const FString& Separator)
	{
		TVector<FString> Words;
		size_t Position = 0;
		FString Word;

		while ((Position = OutString.find(Separator)) != FString::npos)
		{
			Word = OutString.substr(0, Position);
			Words.push_back(Word);
			OutString.erase(0, Position + Separator.length());
		}

		Words.push_back(OutString);

		return Words;
	}
}
