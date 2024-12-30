#pragma once

#include "KeyCode.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

#include <cstdint>

namespace Bloodshot
{
	class IWindow;

	class FInput final : public TStaticSingleton<FInput>
	{
	public:
		FInput(TUniquePtr<IWindow>& Window);

		static bool GetKey(const EKeyCode KeyCode);
		static bool GetKeyDown(const EKeyCode KeyCode);
		static bool GetKeyUp(const EKeyCode KeyCode);

	private:
		TUniquePtr<IWindow>& Window;
	};
}
