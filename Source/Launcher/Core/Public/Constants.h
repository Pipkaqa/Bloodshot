#pragma once

#include "OpenGL/ImGuiHeader.h"

#include <filesystem>

namespace Bloodshot::Launcher
{
	struct IConstants abstract final
	{
		static inline const ImVec2 MinWindowSize = {1000, 500};

		static inline const std::filesystem::path FontPath = "Resources\\Launcher\\Fonts\\Qilavoen.ttf";
		static inline const std::filesystem::path LogoPath = "Resources\\Launcher\\Images\\Logo512.png";
		static inline const std::filesystem::path SettingsIconPath = "Resources\\Launcher\\Images\\SettingsIcon512.png";
		static inline const std::filesystem::path BackIconPath = "Resources\\Launcher\\Images\\BackIcon512.png";
	};
}
