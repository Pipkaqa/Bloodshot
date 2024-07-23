#pragma once

#include "ECS/Component.h"
#include "ECS/Entity.h"
#include "ECS/System.h"
#include "Scene/Scene.h"

namespace Bloodshot::Test
{
	class TestComponent : public Bloodshot::Component<TestComponent>
	{
	public:
		glm::vec3 m_Position = glm::vec3(0.f);
		glm::vec3 m_Rotation = glm::vec3(0.f);
		glm::vec3 m_Scale = glm::vec3(1.f);

	private:
		void BeginPlay() override
		{
			FL_TRACE("Test component begin play call!");
		}

		void EndPlay() override
		{
			FL_TRACE("Test component end play call!");
		}
	};

	class TestEntity : public Bloodshot::Entity<TestEntity>
	{
		void BeginPlay() override
		{
			FL_TRACE("Test entity begin play call!");

			m_TestComponent = ECS::AddComponent<TestComponent>(this);
		}

		void EndPlay() override
		{
			FL_TRACE("Test entity end play call!");

			//ECS::RemoveComponent<TestComponent>(this);
		}

		TestComponent* m_TestComponent = nullptr;
	};

	static void ReloadScene();

	class TestSystem : public Bloodshot::System<TestSystem>
	{
	public:
		TestSystem(TestEntity* testEntity)
			: m_TestEntity(testEntity)
		{
		}

	private:
		void FixedTick() override
		{

		}

		void Tick(float deltaTime) override
		{
			auto testComponent = ECS::GetComponent<TestComponent>(m_TestEntity);

			testComponent->m_Position += glm::vec3(1.f);
		}

		void LateTick(float deltaTime) override
		{
			m_CurrentTicks++;

			if (m_CurrentTicks >= m_TicksToReloadScene)
			{
				FL_CORE_WARNING("Reloading scene...");

				ReloadScene();

				m_SceneReloaded = true;
			}
		}

		TestEntity* m_TestEntity;

		int m_TicksToReloadScene = 1000;
		int m_CurrentTicks = 0;

		bool m_SceneReloaded = false;
	};

	class TestScene : public Bloodshot::Scene
	{
		void BeginPlay() override
		{
			FL_TRACE("Test scene begin play call!");

			m_TestEntity = ECS::Instantiate<TestEntity>();
			m_TestSystem = ECS::AddSystem<TestSystem>(m_TestEntity);

			auto result = ECS::InstantiateMultiple<TestEntity>(16);

			ECS::DestroyMultiple(&result[0], 16);
		}

		void EndPlay() override
		{
			FL_TRACE("Test scene end play call!");

			//ECS::DisableSystem<TestSystem>();
			//ECS::Destroy(m_TestEntity);
		}

		TestEntity* m_TestEntity = nullptr;
		TestSystem* m_TestSystem = nullptr;
	};

	static void ReloadScene()
	{
		SceneManager::LoadScene<TestScene>();
	}
}
