#pragma once

#include <filesystem>
#include <string>

namespace Bloodshot::Launcher
{
	struct FProject final
	{
		std::string Name = "Untitled";

		std::filesystem::path Directory;
		std::filesystem::path ContentDirectory;
		std::filesystem::path StartScenePath;
	};
}
