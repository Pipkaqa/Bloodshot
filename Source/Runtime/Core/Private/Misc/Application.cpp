#include "Misc/Application.h"

namespace Bloodshot
{
	FApplication::FApplication()
	{
		Instance = this;
	}

	void FApplication::Close()
	{
		Instance->bShouldClose = true;
	}
}
