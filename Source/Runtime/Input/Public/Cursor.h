#pragma once

#include <cstdint>

namespace Bloodshot
{
	enum class ECursorLockMode : uint8_t
	{
		None = 0,
		Locked,
		Limited
	};

	// BSTODO: Finish implementation

	struct ICursor final
	{
		ECursorLockMode LockMode = ECursorLockMode::None;
		bool bVisible = true;
	};
}
