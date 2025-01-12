#pragma once

#include "Platform/Platform.h"

namespace Bloodshot
{
	struct ILocalTime abstract final
	{
		NODISCARD static const char* Now();
	};
}
