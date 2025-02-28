#pragma once

#include "BaseUI.h"
#include "Platform/Platform.h"
#include "UI/MainWindow/ProjectsTab.h"
#include "UI/MainWindow/ChangelogTab.h"

#include <string>

namespace Bloodshot::Launcher
{
	class FMainWindow final : public IBaseUI
	{
	public:
		NODISCARD static FMainWindow& GetInstance();

	private:
		FORCEINLINE FMainWindow() 
		{ 
			Setup();
		}

		std::string CurrentPage = "Main";
		std::string CurrentTab = "Projects";

		virtual void Draw() override;

		void SetupMainPage();
		void SetupSettingsPage();
		void Setup();
	};
}
