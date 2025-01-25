#include "HeaderTool.h"

namespace Bloodshot::HeaderTool
{
	FHeaderTool::FHeaderTool(const std::filesystem::path& SourcePath,
		const std::filesystem::path& OutputPath,
		const std::string& Module,
		const bool bOutputToSingleFiles)
		: SourcePath(SourcePath)
		, OutputPath(OutputPath)
		, Module(Module)
		, bOutputToSingleFiles(bOutputToSingleFiles)
	{
		std::filesystem::create_directories(OutputPath);
	}

	void FHeaderTool::Launch()
	{
		if (SourcePath.has_extension() && SourcePath.extension() == ".h")
		{
			ProcessHeaderFile(SourcePath);
		}
		else if (bOutputToSingleFiles)
		{
			ProcessHeaderFilesRecursiveToSingleFiles(SourcePath);
		}
		else
		{
			ProcessHeaderFilesRecursive(SourcePath);
		}
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
		const std::vector<FToken>& Tokens = Tokenizer.Tokenize(SourceCode);

		FHeaderFileInfo HeaderInfo;
		HeaderInfo.ClassInfos = Parser.Parse(Tokens);

		if (!HeaderInfo.ClassInfos.size()) return;

		HeaderInfo.Path = HeaderPath;

		const FGeneratorOutput& GeneratorOutput = Generator.Generate(HeaderInfo);

		SourceOutputStream.close();
		SourceOutputStream.open(OutputPath.string() + HeaderPath.filename().replace_extension("").string() + ".gen.cpp");
		SourceOutputStream << GeneratorOutput.SourceOutput;

		HeaderOutputStream.close();
		HeaderOutputStream.open(OutputPath.string() + HeaderPath.filename().replace_extension("").string() + ".generated.h");
		HeaderOutputStream << GeneratorOutput.HeaderOutput;
	}

	void FHeaderTool::ProcessHeaderFilesRecursiveToSingleFiles(const std::filesystem::path& FolderPath)
	{
		using namespace std::filesystem;

		SourceOutputStream.open(OutputPath.string() + "/" + Module + ".gen.cpp");
		HeaderOutputStream.open(OutputPath.string() + "/" + Module + ".generated.h");

		std::vector<FHeaderFileInfo> HeadersInfo;

		for (const directory_entry& DirectoryEntry : recursive_directory_iterator(FolderPath))
		{
			const path& FilePath = DirectoryEntry.path();

			if (!FilePath.has_extension() || FilePath.extension() != ".h")
			{
				continue;
			}

			std::ifstream InputStream(FilePath);
			if (!InputStream.is_open())
			{
				printf(std::format("Failed to open input header file: {}", FilePath.string()).c_str());
				std::terminate();
			}

			std::stringstream StringStream;
			StringStream << InputStream.rdbuf();
			InputStream.close();

			const std::string& SourceCode = StringStream.str();
			const std::vector<FToken>& Tokens = Tokenizer.Tokenize(SourceCode);

			FHeaderFileInfo HeaderInfo;
			HeaderInfo.ClassInfos = Parser.Parse(Tokens);

			if (!HeaderInfo.ClassInfos.size()) continue;

			HeaderInfo.Path = FilePath;

			HeadersInfo.emplace_back(std::move(HeaderInfo));
		}

		const FGeneratorOutput& GeneratorOutput = Generator.GenerateToSingleFiles(HeadersInfo);

		SourceOutputStream << GeneratorOutput.SourceOutput;
		HeaderOutputStream << GeneratorOutput.HeaderOutput;

		SourceOutputStream.close();
		HeaderOutputStream.close();
	}
}
