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

	FSystemManager::FSystemVector& FSystemManager::GetSystems()
	{
		return Instance->Systems;
	}

	void FSystemManager::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		FSystemVector& Systems = Instance->Systems;

		for (TUniquePtr<ISystem>& System : Systems)
		{
			System.Reset();
		}

		Systems.Clear();
	}

	bool FSystemManager::Contains(const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		FSystemVector& Systems = Instance->Systems;

		return SystemTypeID < Systems.GetSize() && Systems[SystemTypeID];
	}

	InstanceID_t FSystemManager::Store(TUniquePtr<ISystem>&& System, const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		FSystemVector& Systems = Instance->Systems;

		while (SystemTypeID >= Systems.GetSize())
		{
			Systems.Resize(Systems.GetSize() + SystemStorageGrow);
		}

		Systems[SystemTypeID] = std::move(System);

		return SystemTypeID;
	}

	void FSystemManager::Unstore(const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->Systems[SystemTypeID] = nullptr;
	}
}
