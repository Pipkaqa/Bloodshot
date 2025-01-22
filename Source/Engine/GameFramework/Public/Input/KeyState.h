#pragma once

#include <cstdint>

namespace Bloodshot
{
	enum class EKeyState : uint8_t
	{
		Release = 0,
		Press,
		Repeat
	};

	enum class EMouseButtonState : uint8_t
	{
		Press = 0,
		Release
	};
}
