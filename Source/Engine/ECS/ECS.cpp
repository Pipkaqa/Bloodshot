#include "ECS.h"

#include "Debug/Logger.h"

namespace Bloodshot
{
	ECS* ECS::Create(const Config& config, EntityManager* entityManager, ComponentManager* componentManager, SystemManager* systemManager)
	{
		BS_ASSERT(!s_Instance, "An attempt to create another ecs instance");

		s_Instance = new ECS;

		s_Instance->m_Config = config;
		s_Instance->m_EntityManager = entityManager;
		s_Instance->m_ComponentManager = componentManager;
		s_Instance->m_SystemManager = systemManager;

		return s_Instance;
	}

	void ECS::RemoveAllComponents(IEntity* entityInterface)
	{
		BS_PROFILE_FUNCTION();

		s_Instance->m_ComponentManager->RemoveAllComponents(SceneManager::GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
	}

	void ECS::RemoveAllSystems()
	{
		BS_PROFILE_FUNCTION();

		s_Instance->m_SystemManager->RemoveAllSystems(SceneManager::GetCurrentScene()->m_SystemStorage.get());
	}
}
