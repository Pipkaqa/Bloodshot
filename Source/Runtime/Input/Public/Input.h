#pragma once

#include "Core.h"

#include "KeyCode.h"
#include "KeyState.h"

namespace Bloodshot
{
	class IInput final
	{
	public:
		NODISCARD static bool GetKey(const EKeyCode KeyCode);
		NODISCARD static bool GetKeyDown(const EKeyCode KeyCode);
		NODISCARD static bool GetKeyUp(const EKeyCode KeyCode);

	private:
		NODISCARD static bool CheckKeyState(const EKeyCode KeyCode, const EKeyState KeyState);
	};
}
