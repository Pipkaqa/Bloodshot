#include "Util/Util.h"

namespace Bloodshot
{
	std::vector<std::string> Split(std::string_view Line, std::string_view Delimiter)
	{
		size_t WordPosition;
		size_t Offset = 0;

		std::vector<std::string> Words;

		while ((WordPosition = Line.find(' ') != std::string::npos))
		{
			Words.emplace_back(Line.substr(Offset, WordPosition));
			Offset += WordPosition + 1;
		}

		Words.emplace_back(Line.substr(Offset, WordPosition));

		return Words;
	}
}
