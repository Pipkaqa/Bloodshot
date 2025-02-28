#include "Misc/Util.h"

namespace Bloodshot
{
	std::vector<std::string> Split(std::string& String, const std::string& Separator)
	{
		std::vector<std::string> Words;
		size_t Position = 0;
		std::string Word;

		while ((Position = String.find(Separator)) != std::string::npos)
		{
			Word = String.substr(0, Position);
			Words.push_back(Word);
			String.erase(0, Position + Separator.length());
		}

		Words.push_back(String);
		return Words;
	}
}
