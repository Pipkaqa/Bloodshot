#include "Scene.h"

#include "ECS/ECS.h"
#include "Rendering/Renderer.h"

namespace Bloodshot
{
	static void Render(Renderer* renderer)
	{
		//auto begin = ECS::Begin<SpriteComponent>();
		//auto end = ECS::End<SpriteComponent>();
		//
		//for (auto it = begin; it != end; ++it)
		//{
		//	auto transform = it->GetOwner()->GetComponent<TransformComponent>();
		//	renderer->DrawTexture(transform->m_Position, transform->m_Rotation, transform->m_Scale, it->m_Texture, it->m_Color);
		//}
	}

	Scene::Scene()
		: m_EntityStorage(new EntityStorage(this)), m_ComponentStorage(new ComponentStorage(this)), m_SystemStorage(new SystemStorage(this))
	{
	}

	void Scene::BeginSimulation()
	{
	}

	void Scene::EndSimulation()
	{
	}

	void Scene::InternalBeginPlay()
	{
	}

	void Scene::InternalEndPlay()
	{
	}

	void Scene::InternalUpdate(float deltaTime, Renderer* renderer)
	{

		// Fixed time stamp update (maybe bigger than 1 update (2-3, etc...)
		{
			for (auto& system : m_SystemStorage->m_SystemWorkOrder)
			{
				system->FixedTick();
			}

			//m_IntegratedSystemManager->GetSystem<PhysicsSystem>()->Update();
			//Better PhysicsSystem->InternalTick();
		}

		for (auto& system : m_SystemStorage->m_SystemWorkOrder)
		{
			system->Tick(deltaTime);
		}

		for (auto& system : m_SystemStorage->m_SystemWorkOrder)
		{
			system->LateTick(deltaTime);
		}

		//renderer->BeginDrawing();
		//renderer->ClearBackground();


		Render(renderer);
		//Better RenderSystem->InternalTick();
		//than --> m_SystemManager->GetIntegratedSystem<RenderSystem>()->PreRender();
		//than --> m_SystemManager->GetIntegratedSystem<RenderSystem>()->Render();
		//than --> m_SystemManager->GetIntegratedSystem<RenderSystem>()->PostRender();

		//renderer->EndDrawing();

		//OnDrawGizmos...

		//OnGUI...
	}

	void Scene::Dispose()
	{
		m_EntityStorage->Dispose();
		m_ComponentStorage->Dispose();
		m_SystemStorage->Dispose();
	}
}
