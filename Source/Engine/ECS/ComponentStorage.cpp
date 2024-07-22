#include "ComponentStorage.h"

#include "Core/Assert.h"
#include "ECS.h"
#include "Memory/MemoryManager.h"
#include "Scene/Scene.h"

namespace Bloodshot
{
	static ComponentID Lock(std::vector<IComponent*>& components, IComponent* componentInterface)
	{
		size_t componentID = 0;

		for (; componentID < components.size(); componentID++)
		{
			if (!components[componentID])
			{
				components[componentID] = componentInterface;
				return componentID;
			}
		}

		components.resize(components.size() + MemoryManager::GetConfig().m_ComponentsStorageGrow, nullptr);
		components[componentID] = componentInterface;

		return componentID;
	}

	static void Unlock(std::vector<IComponent*>& components, ComponentID componentID)
	{
		FL_CORE_ASSERT((componentID != InvalidComponentTypeID && componentID < components.size()), "An attempt to destroy component that not exists");

		components[componentID] = nullptr;
	}

	ComponentStorage::ComponentStorage(Scene* context)
		: m_Context(context)
	{
		FL_CORE_DEBUG("Creating component storage on scene of type [{0}]...", context->GetTypeName());

		const size_t componentsCount = TypeInfo<IComponent>::GetObjectsCount();

		m_Components.resize(ECS::GetConfig().m_ComponentsStorageGrow, nullptr);
	}

	ComponentStorage::~ComponentStorage()
	{
		m_Context = nullptr;

		m_ComponentMap.clear();
		m_Components.clear();
	}

	void ComponentStorage::Store(IEntity* entityInterface, IComponent* componentInterface, ComponentTypeID componentTypeID)
	{
		EntityID entityID = entityInterface->GetUniqueID();

		FL_CORE_ASSERT(m_ComponentMap[entityID].find(componentTypeID) == m_ComponentMap[entityID].end() || m_ComponentMap[entityID][componentTypeID] == InvalidComponentTypeID, "An attempt to add component to entity that already exists");

		componentInterface->m_UniqueID = Lock(m_Components, componentInterface);

		m_ComponentMap[entityID][componentTypeID] = componentInterface->m_UniqueID;
	}

	void ComponentStorage::Unstore(EntityID entityID, ComponentID componentID, ComponentTypeID componentTypeID)
	{
		Unlock(m_Components, componentID);

		m_ComponentMap[entityID][componentTypeID] = InvalidComponentTypeID;
	}

	void ComponentStorage::Dispose()
	{
		FL_CORE_DEBUG("Destroying component storage on scene of type [{0}]...", m_Context->GetTypeName());

		for (auto& component : m_Components)
		{
			if (!component) continue;

			FL_CORE_TRACE("Destroying component of type [{0}]...", component->GetTypeName());

			component->EndPlay();

			component = nullptr;
		}
	}
}
