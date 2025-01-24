#pragma once

#include "Core.h"

#include <filesystem>

namespace Bloodshot
{
	struct FProject final
	{
		FProject(const std::filesystem::path& Path);

		FString Name = "Untitled";

		std::filesystem::path Directory;
		std::filesystem::path ContentDirectory;
		std::filesystem::path EditorStartupScene;
		std::filesystem::path GameDefaultScene;
	};
}
