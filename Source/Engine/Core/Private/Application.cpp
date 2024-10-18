#include "Application.h"

namespace Bloodshot
{
	FApplication::FApplication(TUniquePtr<IWindow>& Window)
		: Window(Window)
	{
		Instance = this;
	}

	void FApplication::Close()
	{
		Instance->Window->Close();
	}
}
