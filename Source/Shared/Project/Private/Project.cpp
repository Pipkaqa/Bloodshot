#include "Project.h"
#include "ParseTokens.h"

#include <cassert>
#include <format>

namespace
{
	static void ReadLine(std::ifstream& InInputStream, std::string& InLine, const std::function<void(std::string&)>& InVisitor)
	{
		std::getline(InInputStream, InLine);
		InVisitor(InLine);
	}

	static void ReadLine(std::fstream& InInputStream, std::string& InLine, const std::function<void(std::string&)>& InVisitor)
	{
		std::getline(InInputStream, InLine);
		InVisitor(InLine);
	}

	static void ReadContent(std::ifstream& InInputStream, std::string& InLine, const std::function<void(std::string&)>& InVisitor)
	{
		while (std::getline(InInputStream, InLine))
		{
			InVisitor(InLine);
		}
	}

	static void ReadContent(std::fstream& InInputStream, std::string& InLine, const std::function<void(std::string&)>& InVisitor)
	{
		while (std::getline(InInputStream, InLine))
		{
			InVisitor(InLine);
		}
	}
}

namespace Bloodshot::Shared
{
	namespace Private::Project
	{
		void LoadFromFileImpl(Bloodshot::Shared::FProject& OutResult, std::ifstream& InInputStream)
		{
			assert(InInputStream.is_open() && "FProject::LoadFromFile: Failed to open project file for reading");
			std::string Buffer;
			std::string* const Content = (std::string*)&OutResult;
			for (size_t i = 0; i < sizeof(Bloodshot::Shared::FProject) / sizeof(std::string); ++i)
			{
				std::string& ToWrite = Content[i];
				ReadLine(InInputStream, Buffer, [&ToWrite](std::string& InLine)
				{
					bool bNeedWrite = false;
					Bloodshot::Shared::ParseTokens(InLine, " ", [&ToWrite, &bNeedWrite](std::string_view InToken)
					{
						if (bNeedWrite)
						{
							ToWrite = InToken.substr(1, InToken.size() - 2);
							return;
						}
						bNeedWrite = true;
					});
				});
			}
		}

		bool LoadFromDirectoryImpl(Bloodshot::Shared::FProject& OutResult, std::filesystem::directory_iterator InDirIt)
		{
			using namespace std::filesystem;
			directory_iterator EndIt{};
			for (; InDirIt != EndIt; ++InDirIt)
			{
				const std::filesystem::path& Dir = InDirIt->path();
				if (Dir.has_extension() && Dir.extension() == ".bsproject")
				{
					std::ifstream InputStream(Dir);
					LoadFromFileImpl(OutResult, InputStream);
					return true;
				}
			}
			return false;
		}
	}

	FProject::FProject(std::string_view InDirectory)
	{
		*this = LoadFromDirectory(InDirectory);
	}

	FProject::FProject(const std::filesystem::path& InDirectory)
	{
		*this = LoadFromDirectory(InDirectory);
	}

	FProject FProject::LoadFromFile(std::string_view InFilePath)
	{
		FProject Result;
		std::ifstream InputStream(InFilePath.data());
		Private::Project::LoadFromFileImpl(Result, InputStream);
		return Result;
	}

	FProject FProject::LoadFromDirectory(std::string_view InDirectory)
	{
		FProject Result;
		Private::Project::LoadFromDirectoryImpl(Result, std::filesystem::directory_iterator(InDirectory));
		return Result;
	}

	bool FProject::TryLoadFromDirectory(FProject& OutResult, std::string_view InDirectory)
	{
		return Private::Project::LoadFromDirectoryImpl(OutResult, std::filesystem::directory_iterator(InDirectory));
	}

	void FProject::SaveToFile(const std::filesystem::path& InFilePath)
	{
		std::ofstream OutputStream(InFilePath);
		assert(OutputStream.is_open() && "FProject::SaveToFile: Failed to open output file for saving");
		OutputStream << std::format("Name: \"{}\"\nDirectory: \"{}\"\nContentDirectory: \"{}\"\nEditorStartupScene: \"{}\"\nGameDefaultScene: \"{}\"\n",
			Name,
			Directory,
			ContentDirectory,
			EditorStartupScene,
			GameDefaultScene);
	}

	FProject FProjectArchive::Get(const std::string& InName, const std::filesystem::path& InDirectory)
	{
		Stream.open(OutputFileName, std::ios_base::in);
		const std::string& InDirStr = InDirectory.string();
		FProject Result;
		std::string Buffer;
		ReadContent(Stream, Buffer, [this, &InName, &InDirStr, &Result](std::string& InRecord)
		{
			if (InRecord.contains(InName) && InRecord.contains(InDirStr))
			{
				ParseRecord(Result, InRecord);
				return;
			}
		});
		Stream.close();
		return Result;
	}

	void FProjectArchive::Add(const FProject& InProject)
	{
		Stream.open(OutputFileName, std::ios_base::out | std::ios_base::app);
		Stream << std::format("={}?={}?={}?={}?={}\n",
			InProject.Name,
			InProject.Directory,
			InProject.ContentDirectory,
			InProject.EditorStartupScene,
			InProject.GameDefaultScene);
		Stream.close();
	}

	void FProjectArchive::Remove(const FProject& InProject)
	{
		Stream.open(OutputFileName, std::ios_base::in);
		std::string Content;
		std::string Record;
		while (std::getline(Stream, Record))
		{
			if (!Record.contains(InProject.Name) || !Record.contains(InProject.Directory))
			{
				Content += Record;
			}
		}
		Stream.close();
		Stream.open(OutputFileName, std::ios_base::out | std::ios_base::trunc);
		Stream << Content;
		Stream.close();
	}

	bool FProjectArchive::Contains(const FProject& InProject)
	{
		Stream.open(OutputFileName, std::ios_base::in);
		std::string Record;
		while (std::getline(Stream, Record))
		{
			if (Record.contains(InProject.Name) && Record.contains(InProject.Directory))
			{
				Stream.close();
				return true;
			}
		}
		Stream.close();
		return false;
	}

	void FProjectArchive::ForEach(const std::function<void(FProject&)>& InFunc)
	{
		Stream.open(OutputFileName, std::ios_base::in);
		std::string Record;
		while (std::getline(Stream, Record))
		{
			FProject Project;
			ParseRecord(Project, Record);
			InFunc(Project);
		}
		Stream.close();
	}

	void FProjectArchive::ParseRecord(FProject& OutResult, std::string_view InRecord)
	{
		std::string* Content = (std::string*)&OutResult;
		ParseTokens(InRecord, "?", [&Content](std::string_view InToken)
		{
			std::string_view CuttedValue = InToken.substr(1);
			*(Content++) = std::string(CuttedValue.data(), CuttedValue.size());
		});
	}
}
