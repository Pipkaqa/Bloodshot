#include "HeaderTool.h"
#include "Generator.h"
#include "Parser.h"
#include "Tokenizer.h"

#include <format>
#include <fstream>
#include <sstream>

namespace Bloodshot::HeaderTool
{
	FHeaderTool::FHeaderTool(const std::filesystem::path& OutputPath)
		: OutputPath(OutputPath)
	{
		std::filesystem::create_directories(OutputPath);
	}

	void FHeaderTool::ProcessHeaderFilesRecursive(const std::filesystem::path& FolderPath)
	{
		using namespace std::filesystem;

		for (const directory_entry& DirectoryEntry : recursive_directory_iterator(FolderPath))
		{
			const path& FilePath = DirectoryEntry.path();

			if (!FilePath.has_extension() || FilePath.extension() != ".h")
			{
				continue;
			}

			ProcessHeaderFile(FilePath);
		}
	}

	void FHeaderTool::ProcessHeaderFile(const std::filesystem::path& HeaderPath)
	{
		std::ifstream InputStream(HeaderPath);
		if (!InputStream.is_open())
		{
			printf(std::format("Failed to open input header file: {}", HeaderPath.string()).c_str());
			std::terminate();
		}

		std::stringstream StringStream;
		StringStream << InputStream.rdbuf();
		InputStream.close();

		const std::string& SourceCode = StringStream.str();
		StringStream.clear();

		Private::FTokenizer Tokenizer;
		const std::vector<Private::FToken>& Tokens = Tokenizer.Tokenize(SourceCode);

		Private::FParser Parser;
		const std::unordered_map<std::string, Private::FClassInfo>& ClassInfos = Parser.Parse(Tokens);

		if (!ClassInfos.size()) return;

		Private::FGenerator Generator;
		StringStream = Generator.Generate(ClassInfos);

		std::ofstream OutputStream(OutputPath.string() + "/" + HeaderPath.filename().replace_extension("").string() + ".generated.h");
		if (!OutputStream.is_open())
		{
			printf(std::format("Failed to open output file: {}", OutputPath.string()).c_str());
			std::terminate();
		}

		const std::string& GeneratedOutput = StringStream.str();
		OutputStream << GeneratedOutput;
		OutputStream.close();
	}
}
