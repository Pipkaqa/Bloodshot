#pragma once

#include "KeyCode.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	class IWindow;

	class FInput final : public TStaticSingleton<FInput>
	{
	public:
		FInput(TReference<IWindow> Window);

		static bool GetKey(const EKeyCode KeyCode);
		static bool GetKeyDown(const EKeyCode KeyCode);
		static bool GetKeyUp(const EKeyCode KeyCode);

	private:
		TReference<IWindow> Window;
	};
}
