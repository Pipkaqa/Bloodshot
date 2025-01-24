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
		if (bOutputToSingleFiles)
		{
			SourceOutputStream.open(OutputPath.string() + "/" + Module + ".gen.cpp");
			HeaderOutputStream.open(OutputPath.string() + "/" + Module + ".generated.h");
		}

		if (SourcePath.has_extension() && SourcePath.extension() == ".h")
		{
			ProcessHeaderFile(SourcePath);
		}
		else
		{
			ProcessHeaderFilesRecursive(SourcePath);
		}

		if (bOutputToSingleFiles)
		{
			SourceOutputStream.close();
			HeaderOutputStream.close();
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
		StringStream.clear();

		const std::vector<Private::FToken>& Tokens = Tokenizer.Tokenize(SourceCode);
		const std::vector<Private::FClassInfo>& ClassInfos = Parser.Parse(Tokens);

		if (!ClassInfos.size()) return;

		Private::FGeneratorOutput GeneratorOutput = Generator.Generate(ClassInfos, HeaderPath);

		if (!bOutputToSingleFiles)
		{
			SourceOutputStream.close();
			SourceOutputStream.open(OutputPath.string() + HeaderPath.filename().replace_extension("").string() + ".gen.cpp");
			SourceOutputStream << GeneratorOutput.SourceOutput;

			HeaderOutputStream.close();
			HeaderOutputStream.open(OutputPath.string() + HeaderPath.filename().replace_extension("").string() + ".generated.h");
			HeaderOutputStream << GeneratorOutput.HeaderOutput;
		}
		else
		{
			SourceOutputStream << GeneratorOutput.SourceOutput;
			HeaderOutputStream << GeneratorOutput.HeaderOutput;
		}
	}
}
