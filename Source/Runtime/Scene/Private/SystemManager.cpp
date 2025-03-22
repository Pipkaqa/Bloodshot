#include "SystemManager.h"

namespace Bloodshot
{
	FSystemManager& FSystemManager::GetInstance()
	{
		static FSystemManager Instance;
		return Instance;
	}

	void FSystemManager::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();
		FSystemArray& Systems = GetInstance().Systems;
		for (ISystem* const System : Systems)
		{
			DeleteObject(System->GetObject());
		}
		Systems.Clear();
	}
}
