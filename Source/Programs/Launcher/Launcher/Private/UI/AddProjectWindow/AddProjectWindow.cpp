#include "UI/AddProjectWindow/AddProjectWindow.h"

namespace Bloodshot::Launcher
{
	FAddProjectWindow& FAddProjectWindow::GetInstance()
	{
		static FAddProjectWindow Instance;
		return Instance;
	}
}
