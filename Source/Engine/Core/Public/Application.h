#pragma once

#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	class IWindow;

	struct FApplication final : TStaticSingleton<FApplication>
	{
	public:
		FApplication();

		bool bShouldClose = false;

		static void Close();
	};
}
