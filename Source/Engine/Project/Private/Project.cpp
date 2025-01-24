#include "Project.h"

#include <fstream>

namespace Bloodshot
{
	FProject::FProject(const std::filesystem::path& Path)
	{
		std::ifstream InputFileStream(Path.string() + "\\" + (--Path.end())->string() + ".bsproject");
		FString Line;

		auto ReadValue = [&InputFileStream, &Line](auto& Out)
			{
				std::getline(InputFileStream, Line);
				const TVector<FString>& Words = Split(Line, " ");
				if (Words.size() > 1) Out = Words[1];
			};

		ReadValue(Name);
		ReadValue(Directory);
		ReadValue(ContentDirectory);
		ReadValue(EditorStartupScene);
		ReadValue(GameDefaultScene);
	}
}
