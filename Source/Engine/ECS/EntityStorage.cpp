#include "EntityStorage.h"

#include "Entity.h"
#include "Core/Assert.h"
#include "ECS.h"

namespace Bloodshot
{
	namespace
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
	}

	EntityStorage::EntityStorage(Scene* context)
		: m_Context(context)
	{
		FL_CORE_DEBUG("Creating entity storage on scene of type [{0}]...", context->GetTypeName());

		m_Entities.resize(ECS::GetConfig().m_EntitiesStorageGrow, nullptr);
	}

	EntityStorage::~EntityStorage()
	{
		m_Context = nullptr;

		m_Entities.clear();
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

	void EntityStorage::Dispose()
	{
		FL_CORE_DEBUG("Destroying entity storage on scene of type [{0}]...", m_Context->GetTypeName());

		for (auto& entityInterface : m_Entities)
		{
			if (entityInterface == nullptr) continue;

			//FL_CORE_TRACE("Destroying entity of type [{0}]...", entityInterface->GetTypeName());
			//
			//entityInterface->EndPlay();
			//
			//ECS::RemoveAllComponents(entityInterface);

			ECS::Destroy(entityInterface);

			entityInterface = nullptr;
		}
	}
}
