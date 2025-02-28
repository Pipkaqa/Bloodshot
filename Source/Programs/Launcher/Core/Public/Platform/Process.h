#pragma once

#include <string_view>

namespace Bloodshot
{
	struct IProcess final
	{
		static void Create(std::string_view Command);
	};
}
