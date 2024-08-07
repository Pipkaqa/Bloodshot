#include "Application.h"

namespace Bloodshot
{
	FApplication::FApplication(TUniquePtr<IWindow>& WindowRef)
		: WindowRef(WindowRef)
	{
	}

	void FApplication::Close()
	{
		Instance->WindowRef->Close();
	}
}
