#include "Misc/Util.h"

namespace Bloodshot
{
	TArray<FString> Split(FString& OutString, const FString& Separator)
	{
		TArray<FString> Words;
		size_t Position = 0;
		FString Word;

		while ((Position = OutString.find(Separator)) != FString::npos)
		{
			Word = OutString.substr(0, Position);
			Words.PushBack(Word);
			OutString.erase(0, Position + Separator.length());
		}

		Words.PushBack(OutString);

		return Words;
	}
}
