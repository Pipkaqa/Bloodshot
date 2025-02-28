#include "Launcher.h"

namespace Bloodshot::Launcher
{
	NODISCARD FLauncher& Bloodshot::Launcher::FLauncher::GetInstance()
	{
		static FLauncher Instance;
		return Instance;
	}
}
