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
		FORCEINLINE ~FProjectsTab() {}

		NODISCARD static FProjectsTab& GetInstance();

	private:
		FORCEINLINE FProjectsTab()
			: ProjectArchive("Projects.ini")
		{
		}

		Shared::FProjectArchive ProjectArchive;
		std::list<Shared::FProject> Projects;

		bool bNeedReloadProjects = true;
		bool bAddProjectWindowOpened = false;
		bool bNewProjectWindowOpened = false;
		bool bOpenProjectWindowOpened = false;

		virtual void Draw() override;

		void UpdateProjects(const std::filesystem::path& TargetFolderPath);
	};
}
