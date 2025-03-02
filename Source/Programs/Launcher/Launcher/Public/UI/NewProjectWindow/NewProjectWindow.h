#pragma once

#include "BaseUI.h"
#include "Platform/Platform.h"

#include <filesystem>
#include <string>

namespace Bloodshot::Launcher
{
	struct FProject;

	class FNewProjectWindow final : public IBaseUI
	{
	public:
		NODISCARD static FNewProjectWindow& GetInstance();

		FORCEINLINE static void Open()
		{
			GetInstance().ResetInputTextBoxes();
			GetInstance().bOpened = true;
		}

		FORCEINLINE static void Close()
		{
			GetInstance().bOpened = false;
		}

	private:
		bool bOpened = false;

		FORCEINLINE FNewProjectWindow()
		{
			Setup();
		}

		virtual void Draw() override;

		void Setup();

		bool NewProject(const std::string& Name, const std::filesystem::path& TargetFolderPath);

		void ResetInputTextBoxes();
	};
}
