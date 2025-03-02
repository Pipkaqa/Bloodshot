#pragma once

#include "OpenGL/ImGuiHeader.h"
#include "ParseTokens.h"
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

		NODISCARD FORCEINLINE static const std::string& GetProjectsFolderPath()
		{
			return GetInstance().ProjectsFolderPath;
		}

		NODISCARD FORCEINLINE static size_t GetProjectsFolderPathSize()
		{
			return GetInstance().ProjectsFolderPathSize;
		}

	private:
		FORCEINLINE FSettings()
		{
			Load();
		}

		const char* Filename = "Settings.ini";
		static inline constexpr const size_t ProjectsFolderPathSize = 1024;
		std::string ProjectsFolderPath;

		void Save()
		{
			std::ofstream OutputStream(Filename, std::ios::out | std::ios::trunc);
			OutputStream << "ProjectsFolderPath: \"" << ProjectsFolderPath << "\"";
			OutputStream.close();
		}

		FORCEINLINE void Load()
		{
			const bool bSettingsFound = std::filesystem::exists(Filename);

			if (!bSettingsFound)
			{
				ProjectsFolderPath = std::filesystem::current_path().string() + "\\Projects";
			}
			else
			{
				std::ifstream InputStream(Filename);
				std::string Line;
				bool bWrite = false;
				std::getline(InputStream, Line);
				Shared::ParseTokens(Line, " ", [this, &bWrite](std::string_view InToken)
				{
					if (bWrite)
					{
						ProjectsFolderPath = InToken.substr(1, InToken.length() - 2);
					}
					bWrite = true;
				});
				InputStream.close();
			}
		}
	};
}
