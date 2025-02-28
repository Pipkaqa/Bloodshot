#include "Settings.h"

namespace Bloodshot::Launcher
{
	NODISCARD FSettings& FSettings::GetInstance()
	{
		static FSettings Instance;
		return Instance;
	}
}
