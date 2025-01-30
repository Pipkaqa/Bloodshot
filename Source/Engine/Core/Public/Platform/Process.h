#pragma once

#include "Templates/Containers/String.h"

namespace Bloodshot
{
	struct IProcess final
	{
		static void Create(FStringView Command);
	};
}
