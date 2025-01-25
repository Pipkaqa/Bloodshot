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
			const std::filesystem::path& OutputPath,
			const std::string& Module,
			const bool bOutputToSingleFiles);

		void Launch();

	private:
		std::filesystem::path SourcePath;
		std::filesystem::path OutputPath;
		std::string Module;
		bool bOutputToSingleFiles = false;

		std::ofstream SourceOutputStream;
		std::ofstream HeaderOutputStream;

		FTokenizer Tokenizer;
		FParser Parser;
		FGenerator Generator;

		void ProcessHeaderFilesRecursive(const std::filesystem::path& FolderPath);
		void ProcessHeaderFile(const std::filesystem::path& HeaderPath);
		void ProcessHeaderFilesRecursiveToSingleFiles(const std::filesystem::path& FolderPath);

	};
}
