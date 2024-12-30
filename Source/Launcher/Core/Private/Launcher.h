#pragma once

#include "Constants.h"
#include "GUI.h"
#include "Misc.h"
#include "Platform.h"
#include "Widgets.h"

#include <limits>
#include <list>
#include <unordered_map>

namespace Bloodshot::Launcher
{
	class FLauncher final
	{
	public:
		FLauncher();
		~FLauncher();

	private:
		struct FSettings final
		{
			std::filesystem::path ProjectsFolderPath = "Projects";
		};

		struct FDebugInfo final
		{
			ImVec2 LocalMousePosition = {};
			bool bDebugMode = false;
		};

		FGui Gui;
		FSettings Settings;
		FDebugInfo DebugInfo;
		std::list<FProject> ProjectList;

		bool bNeedReloadProjects = true;

		void Init();
		void Run();

		void AddProject();
		void CreateProject();
		void OpenProject(const FProject& Project);
		void UpdateProjectList();

		void LoadFonts();
		void LoadTextures();

		FTopPanel CreateTopPanel();
		void CreateMainPage();
		void CreateSettingsPage();

		void DrawTopPanel();
		void DrawMainPage();
		void DrawSettingsPage();
	};
}
