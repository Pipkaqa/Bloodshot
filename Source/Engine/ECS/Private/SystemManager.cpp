#include "SystemManager.h"
#include "Logging/LoggingMacros.h"
#include "System.h"

namespace Bloodshot
{
	FSystemManager::FSystemManager()
	{
		Instance = this;
	}

	void FSystemManager::Init()
	{
		BS_LOG(Debug, "Creating SystemManager...");
	}

	void FSystemManager::Dispose()
	{
		BS_LOG(Debug, "Destroying SystemManager...");
	}

	std::vector<TReference<ISystem>>& FSystemManager::GetSystems()
	{
		return Instance->SystemVec;
	}

	void FSystemManager::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		std::vector<TReference<ISystem>>& SystemVec = Instance->SystemVec;

		for (TReference<ISystem> System : SystemVec)
		{
			delete System;
		}

		SystemVec.clear();
	}

	bool FSystemManager::Contains(const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		std::vector<TReference<ISystem>>& SystemVec = Instance->SystemVec;

		return SystemTypeID < SystemVec.size() && SystemVec[SystemTypeID];
	}

	InstanceID_t FSystemManager::Store(TReference<ISystem> System, const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		std::vector<TReference<ISystem>>& SystemVec = Instance->SystemVec;

		bool bFreeSpaceFound = false;

		while (SystemTypeID >= SystemVec.size())
		{
			SystemVec.resize(SystemVec.size() + SystemStorageGrow);
		}

		SystemVec[SystemTypeID] = System;

		return SystemTypeID;
	}

	void FSystemManager::Unstore(const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->SystemVec[SystemTypeID] = nullptr;
	}
}
