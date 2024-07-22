#include "EntityManager.h"

#include "Debug/Logger.h"
#include "ECS.h"

#include <format>

namespace Bloodshot
{
	void EntityManager::Destroy(EntityStorage* storage, IEntity* entityInterface)
	{
		auto entityID = entityInterface->m_UniqueID;

		auto storedEntityInterface = storage->m_Entities[entityID];

		FL_CORE_ASSERT(storedEntityInterface, "An attempt to destroy an entity that not exists");

		storage->Unstore(entityID);

		const auto entityTypeID = storedEntityInterface->GetTypeID();

		m_EntityPools.find(entityTypeID)->second->Release(storedEntityInterface);
	}

	void EntityManager::Init()
	{
		FL_CORE_DEBUG("Creating entity manager...");
	}

	void EntityManager::Dispose()
	{
		FL_CORE_DEBUG("Destroying entity manager...");

		for (auto& [id, pool] : m_EntityPools)
		{
			if (!pool) continue;

			FL_CORE_TRACE("Destroying entity pool of type [{0}]...", pool->GetTypeName());

			delete pool;

			pool = nullptr;
		}
	}
}
