#pragma once

#include "BaseUI.h"
#include "GUI.h"
#include "Platform/Platform.h"
#include "Settings.h"
#include "UI/AddProjectWindow/AddProjectWindow.h"
//#include "UI/DebugWindow/DebugWindow.h"
#include "UI/MainWindow/MainWindow.h"
#include "UI/NewProjectWindow/NewProjectWindow.h"
#include "UI/OpenProjectWindow/OpenProjectWindow.h"

#include <list>
#include <memory>

namespace Bloodshot::Launcher
{
	class FLauncher final
	{
	public:
		~FLauncher() {}

		FORCEINLINE static void Launch()
		{
			GetInstance().Run();
		}

	private:
		FLauncher() {}

		std::list<IBaseUI*> InterfaceParts;
		bool bLaunched = false;

		NODISCARD static FLauncher& GetInstance();

		FORCEINLINE void Run()
		{
			if (bLaunched)
			{
				return;
			}

			for (size_t FontSize = 4; FontSize <= 36; ++FontSize)
			{
				FGui::LoadFontFromFile(FSettings::FontPath.string().c_str(), (float)FontSize);
			}

			FGui::LoadTextureFromFile("Logo512", FSettings::LogoPath.string().c_str());
			FGui::LoadTextureFromFile("SettingsIcon512", FSettings::SettingsIconPath.string().c_str());
			FGui::LoadTextureFromFile("BackIcon512", FSettings::BackIconPath.string().c_str());

			InterfaceParts.push_back(&FMainWindow::GetInstance());
			//InterfaceParts.push_back(&FDebugWindow::GetInstance());
			InterfaceParts.push_back(&FAddProjectWindow::GetInstance());
			InterfaceParts.push_back(&FNewProjectWindow::GetInstance());
			InterfaceParts.push_back(&FOpenProjectWindow::GetInstance());

			bLaunched = true;

			while (FGui::IsRunning())
			{
				FGui::BeginRender();
				for (IBaseUI* InterfacePart : InterfaceParts)
				{
					InterfacePart->Draw();
				}
				FGui::EndRender();
			}
		}
	};
}