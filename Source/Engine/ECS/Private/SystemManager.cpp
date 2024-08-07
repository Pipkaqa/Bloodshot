#include "SystemManager.h"

#include "ISystem.h"
#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	void FSystemManager::Init()
	{
		BS_LOG(Debug, "Creating SystemManager...");
	}

	void FSystemManager::Dispose()
	{
		BS_LOG(Debug, "Destroying SystemManager...");
	}

	void FSystemManager::RemoveAllSystems(TUniquePtr<FSystemStorage>& Storage)
	{
		std::vector<ISystem*>& Systems = Storage->GetSystemsVec();

		for (ISystem* const System : Systems)
		{
			delete System;
		}

		Storage->Clear();
	}
}
