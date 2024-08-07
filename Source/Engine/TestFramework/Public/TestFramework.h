#pragma once

#include "Component.h"
#include "ECS.h"
#include "Entity.h"
#include "Logging/LoggingMacros.h"
#include "MathLibrary.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "System.h"

namespace Bloodshot::Test
{
	namespace
	{
		static bool bSceneReloaded = false;

		static void ReloadScene();
	}

	class FTestComponent : public Bloodshot::TComponent<FTestComponent>
	{
	public:
		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Rotation = glm::vec3(0.f);
		glm::vec3 Scale = glm::vec3(1.f);

	private:
		virtual void BeginPlay() override
		{
			//BS_LOG(Trace, "TestComponent BeginPlay() call");
		}

		virtual void EndPlay() override
		{
			//BS_LOG(Trace, "TestComponent EndPlay() call");
		}
	};

	class FTestEntity : public Bloodshot::TEntity<FTestEntity>
	{
		virtual void BeginPlay() override
		{
			//BS_LOG(Trace, "TestEntity BeginPlay() call");

			TestComponent = IECS::AddComponent<FTestComponent>(this);
		}

		virtual void EndPlay() override
		{
			//BS_LOG(Trace, "TestEntity EndPlay() call");

			// BSTODO: #AFTER_EDITOR Now it's gonna provide error, 
			// because all components removes before calling this function, need separate logic
			//IECS::RemoveComponent<FTestComponent>(this);
		}

		FTestComponent* TestComponent = nullptr;
	};

	class FTestSystem : public Bloodshot::TSystem<FTestSystem>
	{
	public:
		FTestSystem() {}

	private:
		virtual void FixedTick() override {}

		virtual void Tick(float DeltaTime) override
		{
			TComponentIterator<FTestComponent> Current = IECS::GetBeginComponentIterator<FTestComponent>();
			TComponentIterator<FTestComponent> End = IECS::GetEndComponentIterator<FTestComponent>();

			for (; Current != End; ++Current)
			{
				Current->Position += 1.f;
			}
		}

		virtual void LateTick(float DeltaTime) override
		{
			if (!bSceneReloaded && FEngineTime::GetGlobalTime() >= 50000.f)
			{
				BS_LOG(Warning, "Reloading Scene...");

				bSceneReloaded = true;

				ReloadScene();
			}
		}
	};

	class FTestScene final : public Bloodshot::IScene
	{
		virtual void BeginPlay() override
		{
			//BS_LOG(Trace, "TestScene BeginPlay() call");

			IECS::InstantiateMultiple<FTestEntity>(5000);

			IECS::AddSystem<FTestSystem>();

			FResourceManager::LoadShader("TestShader", "VertexShader.glsl", "FragmentShader.glsl", true);
		}

		virtual void EndPlay() override
		{
			//BS_LOG(Trace, "TestScene EndPlay() call");
		}
	};

	namespace
	{
		static void ReloadScene()
		{
			FSceneManager::LoadScene<FTestScene>();
		}
	}
}
