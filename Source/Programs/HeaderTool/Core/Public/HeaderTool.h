#pragma once

#include <filesystem>
#include <fstream>

namespace Bloodshot::HeaderTool
{
	class FHeaderTool final
	{
	public:
		FHeaderTool(const std::filesystem::path& OutputPath);

		void ProcessHeaderFilesRecursive(const std::filesystem::path& FolderPath);
		void ProcessHeaderFile(const std::filesystem::path& HeaderPath);

	private:
		std::filesystem::path OutputPath;
	};
}
