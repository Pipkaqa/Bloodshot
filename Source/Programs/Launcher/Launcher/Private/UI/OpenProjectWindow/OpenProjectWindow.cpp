#include "UI/OpenProjectWindow/OpenProjectWindow.h"

namespace Bloodshot::Launcher
{
	FOpenProjectWindow& FOpenProjectWindow::GetInstance()
	{
		static FOpenProjectWindow Instance;
		return Instance;
	}

	void FOpenProjectWindow::Draw()
	{
	}

	void FOpenProjectWindow::Setup()
	{
	}
}
