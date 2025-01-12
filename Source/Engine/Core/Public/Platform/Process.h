#include "Platform/Platform.h"

#include <string>

namespace Bloodshot
{
	struct IProcess abstract final
	{
		static void Create(const std::string& Command);
	};
}
