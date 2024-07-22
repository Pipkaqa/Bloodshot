#include "ECS.h"

#include "Debug/Logger.h"
#include "Scene/SceneManager.h"

namespace Bloodshot
{
	NODISCARD ECS* ECS::Create(const Config& config, EntityManager* entityManager, ComponentManager* componentManager, SystemManager* systemManager)
	{
		FL_CORE_ASSERT(!s_Instance, "An attempt to create another ecs instance");

		if (s_Instance) return s_Instance;

		FL_CORE_DEBUG("Configuring ecs...");

		s_Instance = new ECS;

		s_Instance->m_Config = &config;
		s_Instance->m_EntityManager = entityManager;
		s_Instance->m_ComponentManager = componentManager;
		s_Instance->m_SystemManager = systemManager;

		return s_Instance;
	}

	void ECS::Destroy(IEntity* entityInterface)
	{
		entityInterface->EndPlay();
		s_Instance->RemoveAllComponents(entityInterface);
		s_Instance->m_EntityManager->Destroy(GetCurrentScene()->m_EntityStorage.get(), entityInterface);
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
