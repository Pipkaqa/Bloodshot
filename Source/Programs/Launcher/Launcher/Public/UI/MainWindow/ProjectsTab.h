#pragma once

#include "BaseUI.h"
#include "GUI.h"
#include "Platform/Platform.h"
#include "Platform/Process.h"
#include "Project.h"

#include <filesystem>
#include <list>
#include <string>

namespace Bloodshot::Launcher
{
	class FProjectsTab final : public IBaseUI
	{
		friend class FMainWindow;
		friend class FAddProjectWindow;
		friend class FNewProjectWindow;
		friend class FOpenProjectWindow;

	public:
		NODISCARD static FProjectsTab& GetInstance();

	private:
		FProjectsTab() {}

		std::list<FProject> Projects;

		bool bNeedReloadProjects = true;
		bool bAddProjectWindowOpened = false;
		bool bNewProjectWindowOpened = false;
		bool bOpenProjectWindowOpened = false;

		virtual void Draw() override;

		void Setup();

		FORCEINLINE void AddProject()
		{

		}

		void NewProject(const std::filesystem::path& TargetFolderPath,
			const std::string& Name);

		FORCEINLINE void OpenProject(const FProject& Project)
		{
			IProcess::Create(std::format("{} {}",
				"Bloodshot Engine.exe", "Project:" + Project.Directory.string()).c_str());
		}

		void UpdateProjects(const std::filesystem::path& TargetFolderPath);
	};
}
