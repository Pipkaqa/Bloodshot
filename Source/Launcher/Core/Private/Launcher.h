#pragma once

#include "Constants.h"
#include "GUI.h"
#include "Platform/Platform.h"
#include "Project.h"
#include "Util/Util.h"
#include "Widgets.h"

#include <list>
#include <string>
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
			const char* Filename = "Settings.ini";
			std::shared_ptr<char[]> ProjectsFolderPath = std::make_shared<char[]>(1024);
			size_t ProjectsFolderPathSize = 1024;
		};

		struct FDebugInfo final
		{
			ImVec2 LocalMousePosition = {};
			bool bDebugMode = false;
		};

		struct FMainWindowSpec final
		{
			enum class EPageType : uint8_t
			{
				Main = 0,
				Settings,
			};

			struct FMainPageSpec final
			{
				enum class ETabType : uint8_t
				{
					Projects = 0,
					Changelog
				};

				ETabType CurrentTab = ETabType::Projects;
			};

			EPageType CurrentPage = EPageType::Main;
			FMainPageSpec MainPageSpec{};
			std::unordered_map<EPageType, std::string> PageTypeToIDMap;
		};

		struct FNewProjectWindowSpec final
		{
			enum class EPageType : uint8_t
			{
				Main = 0,
			};

			EPageType CurrentPage = EPageType::Main;
			std::unordered_map<EPageType, std::string> PageTypeToIDMap;
		};

		FGui Gui;
		FSettings Settings;
		FDebugInfo DebugInfo;

		std::list<FProject> ProjectList;
		bool bNeedReloadProjects = true;

		bool bAddProjectWindowOpened = false;
		bool bNewProjectWindowOpened = false;
		bool bOpenProjectWindowOpened = false;

		FMainWindowSpec MainWindowSpec{};
		FNewProjectWindowSpec NewProjectWindowSpec{};

		std::string NewProjectNameHolder = "";

		void Init();
		void Run();

		void LoadSettings();
		void SaveSettings();

		void AddProject();
		void NewProject(const std::filesystem::path& Path = {});
		void OpenProject(const FProject& Project);
		void UpdateProjectList();

		void LoadFonts();
		void LoadTextures();

		FTopPanel CreateTopPanel(std::function<void()>&& CloseButtonFunc = []() {},
			std::function<void()>&& MaximizeButtonFunc = []() {},
			std::function<void()>&& MinimizeButtonFunc = []() {});

		void CreateMainPage();
		void CreateSettingsPage();
		void CreateAddProjectPage();
		void CreateNewProjectPage();
		void CreateOpenProjectPage();

		void DrawTopPanel(const FPage& Page);
		void DrawMainPage();
		void DrawSettingsPage();
		void DrawAddProjectPage();
		void DrawNewProjectPage();
		void DrawOpenProjectPage();
	};
}
