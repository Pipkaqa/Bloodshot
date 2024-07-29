#include "ComponentManager.h"

#include "Debug/Logger.h"

namespace Bloodshot
{
	void ComponentManager::RemoveAllComponents(ComponentStorage* storage, IEntity* entityInterface)
	{
		const auto entityID = entityInterface->m_UniqueID;

		auto& components = storage->m_ComponentMap[entityID];

		for (const auto& [componentTypeID, componentID] : components)
		{
			if (componentID == InvalidComponentID) continue;

			auto componentInterface = storage->m_Components[componentID];

			BS_TRACE("Destroying component of type [{0}]...", componentInterface->GetTypeName());

			componentInterface->EndPlay();

			m_ComponentPools.find(componentTypeID)->second->Release(componentInterface);

			storage->Unstore(entityID, componentID, componentTypeID);
		}
	}

	void ComponentManager::Init()
	{
		BS_DEBUG("Creating component manager...");
	}

	void ComponentManager::Dispose()
	{
		BS_DEBUG("Destroying component manager...");

		for (auto& [id, pool] : m_ComponentPools)
		{
			if (!pool) continue;

			BS_TRACE("Destroying component pool of type [{0}]...", pool->GetTypeName());

			delete pool;
		}

		m_ComponentPools.clear();
	}
}
