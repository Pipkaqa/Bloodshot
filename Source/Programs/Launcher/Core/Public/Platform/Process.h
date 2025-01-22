#pragma once

#include <string>

namespace Bloodshot
{
	struct IProcess abstract final
	{
		static void Create(std::string_view Command);
	};
}
