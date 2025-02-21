#pragma once

#include "Containers/StringFwd.h"

namespace Bloodshot
{
	struct IProcess final
	{
		static void Create(FStringView Command);
	};
}
