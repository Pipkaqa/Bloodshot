#pragma once

#include "Misc/Util.h"
#include "OpenGL/ImGuiHeader.h"
#include "Platform/Platform.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string_view>

namespace Bloodshot::Launcher
{
	class FSettings final
	{
		friend class FMainWindow;

	public:
		FORCEINLINE ~FSettings()
		{
			Save();
		}

		static inline const ImVec2 MinWindowSize = {1000, 500};

		static inline const std::filesystem::path FontPath = "Resources\\Launcher\\Fonts\\Qilavoen.ttf";
		static inline const std::filesystem::path LogoPath = "Resources\\Launcher\\Images\\Logo512.png";
		static inline const std::filesystem::path SettingsIconPath = "Resources\\Launcher\\Images\\SettingsIcon512.png";
		static inline const std::filesystem::path BackIconPath = "Resources\\Launcher\\Images\\BackIcon512.png";

		NODISCARD static FSettings& GetInstance();

		NODISCARD FORCEINLINE static const char* GetProjectsFolderPath()
		{
			return GetInstance().ProjectsFolderPath.get();
		}

		NODISCARD FORCEINLINE static size_t GetProjectsFolderPathSize()
		{
			return GetInstance().ProjectsFolderPathSize;
		}

		FORCEINLINE static void SetProjectsFolderPath(std::string_view NewProjectsFolderPath)
		{
			assert(NewProjectsFolderPath.length() <= 1024 && "FSettings::SetProjectsFolderPath: NewProjectsFolderPath was bigger than 1024");
			strcpy(GetInstance().ProjectsFolderPath.get(), NewProjectsFolderPath.data());
		}

	private:
		FORCEINLINE FSettings()
		{
			Load();
		}

		const char* Filename = "Settings.ini";
		std::shared_ptr<char[]> ProjectsFolderPath = std::make_shared<char[]>(1024);
		size_t ProjectsFolderPathSize = 1024;

		void Save()
		{
			std::ofstream OutputStream(Filename, std::ios::out | std::ios::trunc);
			OutputStream << "ProjectsFolderPath: " << ProjectsFolderPath.get();
			OutputStream.close();
		}

		FORCEINLINE void Load()
		{
			const bool bSettingsFound = std::filesystem::exists(Filename);

			if (!bSettingsFound)
			{
				strcpy_s(ProjectsFolderPath.get(), ProjectsFolderPathSize, "Projects");
			}
			else
			{
				std::ifstream InputStream(Filename);
				std::string Line;

				auto ReadValue = [&InputStream, &Line](char* Out, std::function<void(char*, const std::vector<std::string>&)> HowToRead)
				{
					std::getline(InputStream, Line);
					const std::vector<std::string>& Words = Split(Line, " ");
					HowToRead(Out, Words);
				};

				ReadValue(ProjectsFolderPath.get(), [this](char* Out, const std::vector<std::string>& Words)
				{
					if (Words.size() > 1)
						strcpy_s(Out, ProjectsFolderPathSize, Words[1].c_str());
				});

				InputStream.close();
			}
		}
	};
}
