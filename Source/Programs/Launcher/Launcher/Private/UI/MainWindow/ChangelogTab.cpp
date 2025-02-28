#include "UI/MainWindow/ChangelogTab.h"

namespace Bloodshot::Launcher
{
	FChangelogTab& FChangelogTab::GetInstance()
	{
		static FChangelogTab Instance;
		return Instance;
	}

	void FChangelogTab::Draw()
	{
	}

	void FChangelogTab::Setup()
	{
	}
}
