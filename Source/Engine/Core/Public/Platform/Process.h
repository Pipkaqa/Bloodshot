#pragma once

#include "Templates/Containers/String.h"

namespace Bloodshot
{
	struct IProcess abstract final
	{
		static void Create(FStringView Command);
	};
}
