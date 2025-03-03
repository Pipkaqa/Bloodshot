#pragma once

#include "Core.h"

#include <filesystem>
#include <fstream>

namespace Bloodshot
{
	// BSTODO: Serialization

	struct FProject final
	{
		// BSTODO: Temp

		NODISCARD FORCEINLINE static FProject Load(const std::filesystem::path& Path)
		{
			FProject Project;

			std::ifstream InputFileStream(Path.string() + "\\" + (--Path.end())->string() + ".bsproject");
			std::string Line;

			auto ReadValue = [&InputFileStream, &Line](auto& Out)
			{
				std::getline(InputFileStream, Line);
				FString Str(Line.c_str());
				TArray<FString> Words;
				Str.ParseIntoArray(Words, " ");
				if (Words.GetSize() > 1) Out = Words[1].GetData();
			};

			ReadValue(Project.Name);
			ReadValue(Project.Directory);
			ReadValue(Project.ContentDirectory);
			ReadValue(Project.EditorStartupScene);
			ReadValue(Project.GameDefaultScene);

			return Project;
		}

		FString Name = "Untitled";
		std::filesystem::path Directory;
		std::filesystem::path ContentDirectory;
		std::filesystem::path EditorStartupScene;
		std::filesystem::path GameDefaultScene;
	};
}
