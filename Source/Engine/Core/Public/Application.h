#pragma once

#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"
#include "Window.h"

namespace Bloodshot
{
	struct FApplication final : TStaticSingleton<FApplication>
	{
	public:
		FApplication(TUniquePtr<IWindow>& WindowRef);

		TUniquePtr<IWindow>& WindowRef;

		static void Close();
	};
}
