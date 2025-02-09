#pragma once

#include "Generator.h"
#include "Parser.h"
#include "Tokenizer.h"

#include <filesystem>
#include <fstream>

namespace Bloodshot::HeaderTool
{
	class FHeaderTool final
	{
	public:
		FHeaderTool(const std::filesystem::path& SourcePath,
			const std::filesystem::path& OutputPath);

		void Launch();

	private:
		std::string CurrentModuleName;

		std::filesystem::path SourcePath;
		std::filesystem::path OutputPath;

		std::ofstream SourceOutputStream;
		std::ofstream HeaderOutputStream;

		FTokenizer Tokenizer;
		FParser Parser;
		FGenerator Generator;

		void ProcessHeaderFilesRecursive(const std::filesystem::path& FolderPath);
		void ProcessHeaderFile(const std::filesystem::path& HeaderPath);
	};
}
