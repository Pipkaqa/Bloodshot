#include "Input.h"
#include "Window.h"

namespace Bloodshot
{
	FInput::FInput(TUniquePtr<IWindow>& Window)
		: Window(Window)
	{
		Instance = this;
	}

	bool FInput::GetKey(const EKeyCode KeyCode)
	{
		if (Instance->Window->GetKeyState(KeyCode) == EKeyState::Repeat)
		{
			return true;
		}

		return false;
	}

	bool FInput::GetKeyDown(const EKeyCode KeyCode)
	{
		if (Instance->Window->GetKeyState(KeyCode) == EKeyState::Press)
		{
			return true;
		}

		return false;
	}

	bool FInput::GetKeyUp(const EKeyCode KeyCode)
	{
		if (Instance->Window->GetKeyState(KeyCode) == EKeyState::Release)
		{
			return true;
		}

		return false;
	}
}
