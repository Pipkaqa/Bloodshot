#include "EntityManager.h"

#include "Debug/Logger.h"
#include "Entity.h"

namespace Bloodshot
{
	void EntityManager::Destroy(EntityStorage* storage, IEntity* entityInterface)
	{
		auto entityID = entityInterface->m_UniqueID;

		auto& entities = storage->m_Entities;

		BS_ASSERT(entityID != InvalidEntityTypeID && entityID < entities.size(), "An attempt to destroy not existing entity");

		auto storedEntityInterface = storage->m_Entities[entityID];

		BS_ASSERT(storedEntityInterface, "An attempt to destroy not existing entity");

		BS_TRACE("Destroying entity of type [{0}]...", entityInterface->GetTypeName());

		entityInterface->EndPlay();

		storage->Unstore(entityID);

		const auto entityTypeID = storedEntityInterface->GetTypeID();

		m_EntityPools.find(entityTypeID)->second->Release(storedEntityInterface);
	}

	void EntityManager::Init()
	{
		BS_DEBUG("Creating entity manager...");
	}

	void EntityManager::Dispose()
	{
		BS_DEBUG("Destroying entity manager...");

		for (auto& [id, pool] : m_EntityPools)
		{
			if (!pool) continue;

			BS_TRACE("Destroying entity pool of type [{0}]...", pool->GetTypeName());

			delete pool;
		}

		m_EntityPools.clear();
	}
}
