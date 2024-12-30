#include "HeaderTool.h"

#include <iostream>

int main(int Argc, char** Argv)
{
	Bloodshot::HeaderTool::FHeaderTool HeaderTool;

	std::filesystem::path Path;

	if (Argc == 1)
	{
		Path = Argv[0];
	}
	else if (Argc == 2)
	{
		Path = Argv[1];
	}

	Path = "C:\\Users\\Pipka\\Desktop\\Bloodshot\\VsOut\\Build\\x64-Debug\\Binaries\\Debug\\Projects\\Bloodshot Test Project\\Assets";

	if (Path.empty())
	{
		return 2;
	}

	if (Path.has_extension() && Path.extension() == ".h")
	{
		HeaderTool.ParseHeaderFile(Path);
		return 0;
	}

	for (const std::filesystem::path& File : std::filesystem::recursive_directory_iterator(Path))
	{
		if (File.has_extension() && File.extension() == ".h")
		{
			HeaderTool.ParseHeaderFile(File);
		}
	}
}