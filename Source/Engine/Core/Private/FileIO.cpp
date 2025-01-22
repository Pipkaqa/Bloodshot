#include "FileIO.h"
#include "AssertionMacros.h"

#include <fstream>
#include <sstream>

namespace Bloodshot
{
	FString IFileIO::ReadFile(FStringView Path)
	{
		std::ifstream File;
		File.open(Path.data());

		BS_CHECK(File.is_open(), "Failed to open and read file");

		std::stringstream StringStream;
		StringStream << File.rdbuf();

		return StringStream.str();
	}
}
