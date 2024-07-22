#pragma once

#include "Utility/Utility.h"

namespace Bloodshot
{
	class Locale abstract final
	{
	public:
		NODISCARD static const char* GetLocalTime();

	private:
		static char s_Buffer[128];
	};
}
