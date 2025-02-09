#include "SystemManager.h"

namespace Bloodshot
{
	FSystemManager::FSystemManager()
	{
		Instance = this;
	}

	void FSystemManager::Init()
	{
		BS_LOG(Debug, "Creating FSystemManager...");
	}

	void FSystemManager::Dispose()
	{
		BS_LOG(Debug, "Destroying FSystemManager...");
	}

	FSystemManager::FSystemArray& FSystemManager::GetSystems()
	{
		return Instance->Systems;
	}

	void FSystemManager::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		FSystemArray& Systems = Instance->Systems;

		for (TReference<ISystem> System : Systems)
		{
			DeleteObject(System->GetObject());
		}

		Systems.Clear();
	}

	bool FSystemManager::Contains(const FTypeID SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		FSystemArray& Systems = Instance->Systems;

		return SystemTypeID < Systems.GetSize() && Systems[SystemTypeID];
	}

	FInstanceID FSystemManager::Store(TReference<ISystem> System, const FTypeID SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		FSystemArray& Systems = Instance->Systems;

		while (SystemTypeID >= Systems.GetSize())
		{
			Systems.Resize(Systems.GetSize() + SystemStorageGrow);
		}

		Systems[SystemTypeID] = System;

		FInstanceID InstanceID;
		InstanceID.Value = SystemTypeID.Value;

		return InstanceID;
	}

	void FSystemManager::Unstore(const FTypeID SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->Systems[SystemTypeID] = nullptr;
	}
}
