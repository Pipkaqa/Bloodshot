#include "ECS.h"

#include "Debug/Logger.h"
#include "Scene/SceneManager.h"

namespace Bloodshot
{
	ECS* ECS::Create(const Config& config, EntityManager* entityManager, ComponentManager* componentManager, SystemManager* systemManager)
	{
		FL_CORE_ASSERT(!s_Instance, "An attempt to create another ecs instance");

		if (s_Instance) return s_Instance;

		s_Instance = new ECS;

		s_Instance->m_Config = &config;
		s_Instance->m_EntityManager = entityManager;
		s_Instance->m_ComponentManager = componentManager;
		s_Instance->m_SystemManager = systemManager;

		return s_Instance;
	}

	void ECS::Init()
	{
		FL_CORE_DEBUG("Creating ecs api...");
	}

	void ECS::Dispose()
	{
		FL_CORE_DEBUG("Destroying ecs api...");
	}

	void ECS::RemoveAllComponents(const IEntity* entityInterface)
	{
		s_Instance->m_ComponentManager->RemoveAllComponents(GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
	}

	Scene* ECS::GetCurrentScene()
	{
		return SceneManager::s_Instance->m_ActiveScene;
	}
}
