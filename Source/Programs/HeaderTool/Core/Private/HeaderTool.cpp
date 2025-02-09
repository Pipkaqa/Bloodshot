#include "HeaderTool.h"

namespace Bloodshot::HeaderTool
{
	FHeaderTool::FHeaderTool(const std::filesystem::path& SourcePath,
		const std::filesystem::path& OutputPath)
		: SourcePath(SourcePath)
		, OutputPath(OutputPath)
	{
		std::filesystem::create_directories(OutputPath);
	}

	void FHeaderTool::Launch()
	{
		if (SourcePath.has_extension() && SourcePath.extension() == ".h")
		{
			ProcessHeaderFile(SourcePath);
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

			if (FilePath.filename().string() == "CMakeLists.txt")
			{
				CurrentModuleName = (----FilePath.end())->string();
				continue;
			}

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

		const std::string& FullOutputPathStr = OutputPath.string() + "/" + CurrentModuleName;
		const std::string& CurrentHeaderFileNameStr = HeaderPath.filename().replace_extension("").string();

		std::filesystem::create_directories(FullOutputPathStr);

		SourceOutputStream.close();
		SourceOutputStream.open(FullOutputPathStr + "/" + CurrentHeaderFileNameStr + ".gen.cpp");
		SourceOutputStream << GeneratorOutput.SourceOutput;

		HeaderOutputStream.close();
		HeaderOutputStream.open(FullOutputPathStr + "/" + CurrentHeaderFileNameStr + ".generated.h");
		HeaderOutputStream << GeneratorOutput.HeaderOutput;
	}
}
