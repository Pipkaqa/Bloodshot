#include "Scene.h"

#include "ECS/ECS.h"
#include "Rendering/Renderer.h"
#include "Rendering/Window.h"

namespace Bloodshot
{
	void Scene::BeginSimulation()
	{

	}

	void Scene::EndSimulation()
	{

	}

	void Scene::InternalBeginPlay()
	{
		//for (auto& entityInterface : m_EntityStorage->m_Entities)
		//{
		//	if (!entityInterface) continue;
		//
		//	entityInterface->BeginPlay();
		//}
		//
		//for (auto& componentInterface : m_ComponentStorage->m_Components)
		//{
		//	if (!componentInterface) continue;
		//
		//	componentInterface->BeginPlay();
		//}
	}

	void Scene::InternalEndPlay()
	{
		auto& entities = m_EntityStorage->m_Entities;

		//for (auto& entityInterface : entities)
		//{
		//	if (!entityInterface) continue;
		//
		//	entityInterface->EndPlay();
		//}
		//
		//for (auto& componentInterface : m_ComponentStorage->m_Components)
		//{
		//	if (!componentInterface) continue;
		//
		//	componentInterface->EndPlay();
		//}

		ECS::DestroyMultiple(&m_EntityStorage->m_Entities[0], m_EntityStorage->m_Entities.size());
		ECS::RemoveAllSystems();
	}

	void Scene::InternalUpdate(float deltaTime, Renderer* renderer, Window* window)
	{
		//TODO: fixed time stamp
		//
		for (auto& system : m_SystemStorage->m_SystemWorkOrder)
		{
			//BSTODO: temp, but doesnt work (reloading scene = delete systems = UB
			if (!system) continue;
			system->FixedTick();
		}
		//
		for (auto& system : m_SystemStorage->m_SystemWorkOrder)
		{
			//BSTODO: temp, but doesnt work (reloading scene = delete systems = UB
			if (!system) continue;
			system->Tick(deltaTime);
		}

		for (auto& system : m_SystemStorage->m_SystemWorkOrder)
		{
			//BSTODO: temp, but doesnt work (reloading scene = delete systems = UB
			if (!system) continue;
			system->LateTick(deltaTime);
		}

		renderer->ClearBackground();

		window->SwapBuffers();
		window->PollEvents();

		//OnDrawGizmos...

		//OnGUI...
	}
}
