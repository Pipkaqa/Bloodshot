#include "SystemManager.h"
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

		Systems.clear();
	}

	bool FSystemManager::Contains(const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		FSystemVector& Systems = Instance->Systems;

		return SystemTypeID < Systems.size() && Systems.at(SystemTypeID);
	}

	InstanceID_t FSystemManager::Store(TUniquePtr<ISystem>&& System, const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		FSystemVector& Systems = Instance->Systems;

		while (SystemTypeID >= Systems.size())
		{
			Systems.resize(Systems.size() + SystemStorageGrow);
		}

		Systems.at(SystemTypeID) = std::move(System);

		return SystemTypeID;
	}

	void FSystemManager::Unstore(const TypeID_t SystemTypeID)
	{
		BS_PROFILE_FUNCTION();

		Instance->Systems.at(SystemTypeID) = nullptr;
	}
}
