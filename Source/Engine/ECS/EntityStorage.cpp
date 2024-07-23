#include "EntityStorage.h"

#include "Core/Assert.h"
#include "ECS.h"
#include "Entity.h"

namespace Bloodshot
{
	static EntityID Lock(std::vector<IEntity*>& entities, IEntity* entityInterface)
	{
		size_t entityID = 0;

		for (; entityID < entities.size(); entityID++)
		{
			if (entities[entityID] == nullptr)
			{
				entities[entityID] = entityInterface;
				return entityID;
			}
		}

		entities.resize(entities.size() + ECS::GetConfig().m_EntitiesStorageGrow, nullptr);
		entities[entityID] = entityInterface;

		return entityID;
	}

	EntityStorage::EntityStorage(Scene* context)
		: m_Context(context)
	{
		FL_CORE_DEBUG("Creating entity storage on scene of type [{0}]...", context->GetTypeName());

		m_Entities.resize(ECS::GetConfig().m_EntitiesStorageGrow, nullptr);
	}

	EntityStorage::~EntityStorage()
	{
		FL_CORE_DEBUG("Destroying entity storage on scene of type [{0}]...", m_Context->GetTypeName());
	}

	void EntityStorage::Store(IEntity* entityInterface)
	{
		entityInterface->m_UniqueID = Lock(m_Entities, entityInterface);
	}

	void EntityStorage::Unstore(EntityID entityID)
	{
		FL_CORE_ASSERT((entityID != InvalidEntityTypeID && entityID < m_Entities.size()), "An attempt to destroy entity that not exists");

		m_Entities[entityID] = nullptr;
	}
}
