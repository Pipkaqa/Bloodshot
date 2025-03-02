#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

namespace Bloodshot::Shared
{
	struct FProject;

	namespace Private::Project
	{
		void LoadFromFileImpl(Bloodshot::Shared::FProject& OutResult, std::ifstream& InInputStream);
		bool LoadFromDirectoryImpl(Bloodshot::Shared::FProject& OutResult, std::filesystem::directory_iterator InDirIt);
	}

	struct FProject final
	{
		FProject() {}
		FProject(std::string_view InDirectory);
		FProject(const std::filesystem::path& InDirectory);

		std::string Name;
		std::string Directory;
		std::string ContentDirectory;
		std::string EditorStartupScene;
		std::string GameDefaultScene;

		static FProject LoadFromFile(std::string_view InFilePath);

		template<typename T>
			requires std::_Is_any_path<T>
		static FProject LoadFromFile(const T& InFilePath)
		{
			FProject Result;
			std::ifstream InputStream(InFilePath);
			Private::Project::LoadFromFileImpl(Result, InputStream);
			return Result;
		}

		static FProject LoadFromDirectory(std::string_view InDirectory);

		template<typename T>
			requires std::_Is_any_path<T>
		static FProject LoadFromDirectory(const T& InDirectory)
		{
			FProject Result;
			Private::Project::LoadFromDirectoryImpl(Result, std::filesystem::directory_iterator(InDirectory));
			return Result;
		}

		static bool TryLoadFromDirectory(FProject& OutResult, std::string_view InDirectory);

		template<typename T>
			requires std::_Is_any_path<T>
		static bool TryLoadFromDirectory(FProject& OutResult, const T& InDirectory)
		{
			return Private::Project::LoadFromDirectoryImpl(OutResult, std::filesystem::directory_iterator(InDirectory));
		}

		void SaveToFile(const std::filesystem::path& InFilePath);
	};

	class FProjectArchive final
	{
	public:
		FProjectArchive(std::string_view InOutputFileName)
			: OutputFileName(InOutputFileName)
		{
		}

		FProject Get(const std::string& Name, const std::filesystem::path& Directory);
		void Add(const FProject& InProject);
		void Remove(const FProject& InProject);
		bool Contains(const FProject& InProject);
		void ForEach(const std::function<void(FProject&)>& Func);

	private:
		std::fstream Stream;
		std::string OutputFileName;

		void ParseRecord(FProject& OutResult, std::string_view InRecord);
	};
}
