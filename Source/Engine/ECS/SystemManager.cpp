#include "SystemManager.h"

#include "Debug/Logger.h"

namespace Bloodshot
{
	void SystemManager::Init()
	{
		FL_CORE_DEBUG("Creating system manager...");
	}

	void SystemManager::Dispose()
	{
		FL_CORE_DEBUG("Destroying system manager...");
	}

	//void SystemManager::EndSimulation()
	//{
	//	//for (auto& [id, system] : m_Systems)
	//	//{
	//	//	system->EndPlay();
	//	//}
	//}
}
