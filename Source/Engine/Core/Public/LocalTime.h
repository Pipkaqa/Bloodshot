#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	struct ILocalTime final
	{
		NODISCARD static const char* Now();
	};
}
