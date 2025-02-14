#include "Misc/Application.h"

namespace Bloodshot
{
	FApplication& FApplication::GetInstance()
	{
		static FApplication Instance;
		return Instance;
	}
}
