#include "Input.h"
#include "Window.h"

namespace Bloodshot
{
	bool IInput::GetKey(const EKeyCode KeyCode)
	{
		if (CheckKeyState(KeyCode, EKeyState::Repeat))
		{
			return true;
		}

		return false;
	}

	bool IInput::GetKeyDown(const EKeyCode KeyCode)
	{
		if (CheckKeyState(KeyCode, EKeyState::Press))
		{
			return true;
		}

		return false;
	}

	bool IInput::GetKeyUp(const EKeyCode KeyCode)
	{
		if (CheckKeyState(KeyCode, EKeyState::Release))
		{
			return true;
		}

		return false;
	}

	bool IInput::CheckKeyState(const EKeyCode KeyCode, const EKeyState KeyState)
	{
		return IWindow::GetInstance()->GetKeyState(StaticCast<int>(KeyCode)) == StaticCast<int>(KeyState);
	}
}
