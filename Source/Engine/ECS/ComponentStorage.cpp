#include "ComponentStorage.h"

#include "ECS.h"

namespace Bloodshot
{
	static ComponentID Lock(std::vector<IComponent*>& components, IComponent* componentInterface)
	{
		size_t componentID = 0;

		size_t componentsSize = components.size();

		for (; componentID < componentsSize; componentID++)
		{
			if (!components[componentID])
			{
				components[componentID] = componentInterface;

				return componentID;
			}
		}

		components.resize(componentsSize + ECS::GetConfig().m_ComponentsStorageGrow, nullptr);
		components[componentID] = componentInterface;

		return componentID;
	}

	static void Unlock(std::vector<IComponent*>& components, ComponentID componentID)
	{
		components[componentID] = nullptr;
	}

	ComponentStorage::ComponentStorage(Scene* context)
		: m_Context(context)
	{
		BS_DEBUG("Creating component storage on scene of type [{0}]...", context->GetTypeName());

		const size_t componentsCount = TypeInfo<IComponent>::GetObjectsCount();

		m_Components.resize(ECS::GetConfig().m_ComponentsStorageGrow, nullptr);
	}

	ComponentStorage::~ComponentStorage()
	{
		BS_DEBUG("Destroying component storage on scene of type [{0}]...", m_Context->GetTypeName());
	}

	void ComponentStorage::Store(IEntity* entityInterface, IComponent* componentInterface, ComponentTypeID componentTypeID)
	{
		EntityID entityID = entityInterface->GetUniqueID();

		//BSTODO: Move check in component manager
		BS_ASSERT(m_ComponentMap[entityID].find(componentTypeID) == m_ComponentMap[entityID].end()
			|| m_ComponentMap[entityID][componentTypeID] == InvalidComponentID,
			"An attempt to add already existing component to entity");

		componentInterface->m_UniqueID = Lock(m_Components, componentInterface);

		m_ComponentMap[entityID][componentTypeID] = componentInterface->m_UniqueID;
	}

	void ComponentStorage::Unstore(EntityID entityID, ComponentID componentID, ComponentTypeID componentTypeID)
	{
		Unlock(m_Components, componentID);

		m_ComponentMap[entityID][componentTypeID] = InvalidComponentID;
	}
}
