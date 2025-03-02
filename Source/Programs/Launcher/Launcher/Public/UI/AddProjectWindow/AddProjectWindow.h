#pragma once

#include "BaseUI.h"
#include "Platform/Platform.h"

#include <filesystem>

namespace Bloodshot::Launcher
{
	class FAddProjectWindow final : public IBaseUI
	{
	public:
		NODISCARD static FAddProjectWindow& GetInstance();

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

		FORCEINLINE FAddProjectWindow()
		{
			Setup();
		}

		virtual void Draw() override;

		void AddProject(const char* InDirectory);

		void Setup();

		void ResetInputTextBoxes();
	};
}
