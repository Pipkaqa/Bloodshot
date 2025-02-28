#pragma once

#include "BaseUI.h"
#include "Platform/Platform.h"
#include "Settings.h"

#include <filesystem>
#include <optional>
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

		std::optional<FProject> NewProject(const std::string& Name,
			const std::filesystem::path& TargetFolderPath = FSettings::GetProjectsFolderPath());

		void Setup();
	};
}
