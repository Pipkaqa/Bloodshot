#pragma once

#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"
#include "Window.h"

namespace Bloodshot
{
	struct FApplication final : TStaticSingleton<FApplication>
	{
	public:
		FApplication(TUniquePtr<IWindow>& Window);

		TUniquePtr<IWindow>& Window;

		static void Close();
	};
}
