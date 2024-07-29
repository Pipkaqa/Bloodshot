#include "FileIO.h"

#include "Debug/Logger.h"

#include <sstream>

namespace Bloodshot
{
	std::string FileIO::ReadFile(std::string_view path)
	{
		std::ifstream ifstream;
		ifstream.open(path.data());

		BS_CHECK_ERROR(ifstream.is_open(), "Failed to open file for reading!");

		std::stringstream stream;
		stream << ifstream.rdbuf();

		return stream.str();
	}
}
