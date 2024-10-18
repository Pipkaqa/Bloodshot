#include "FileIO.h"
#include "AssertionMacros.h"

#include <sstream>

namespace Bloodshot
{
	std::string IFileIO::ReadFile(std::string_view Path)
	{
		std::ifstream Stream;
		Stream.open(Path.data());

		BS_CHECK(Stream.is_open(), "Failed to open and read file");

		std::stringstream SStream;
		SStream << Stream.rdbuf();

		return SStream.str();
	}
}
