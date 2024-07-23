#pragma once

#include "Component.h"
#include "IComponent.h"
#include "IEntity.h"

#include <vector>

namespace Bloodshot
{
	class Scene;

	class ComponentStorage final
	{
		ECS_PART;

	public:
		ComponentStorage(Scene* context);
		~ComponentStorage();

	private:
		Scene* m_Context;

		std::unordered_map<EntityID, std::unordered_map<ComponentTypeID, ComponentID>> m_ComponentMap;
		std::vector<IComponent*> m_Components;

		void Store(IEntity* entityInterface, IComponent* componentInterface, ComponentTypeID componentTypeID);
		void Unstore(EntityID entityID, ComponentID componentID, ComponentTypeID componentTypeID);

		friend class Scene;
	};
}
