#include "SystemManager.h"

#include "Debug/Logger.h"

namespace Bloodshot
{
	void SystemManager::Init()
	{
		BS_DEBUG("Creating system manager...");
	}

	void SystemManager::Dispose()
	{
		BS_DEBUG("Destroying system manager...");
	}

	void SystemManager::RemoveAllSystems(SystemStorage* storage)
	{
		auto& systems = storage->m_Systems;

		for (auto [systemTypeID, systemInterface] : systems)
		{
			if (!systemInterface) continue;

			storage->Unstore(systemInterface);

			delete systemInterface;
		}

		systems.clear();
	}
}
