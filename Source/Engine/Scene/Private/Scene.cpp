#include "Scene.h"

#include "ECS.h"
#include "ISystem.h"
#include "Renderer.h"
#include "Window.h"

namespace Bloodshot
{
	void IScene::BeginSimulation()
	{
	}

	void IScene::EndSimulation()
	{
	}

	void IScene::InternalBeginPlay()
	{
	}

	void IScene::InternalEndPlay()
	{
		IECS::DestroyMultiple(EntityStorage->EntitiesVec);
		IECS::RemoveAllSystems();
	}

	void IScene::InternalUpdate(float DeltaTime, TUniquePtr<IRenderer>& Renderer, TUniquePtr<IWindow>& Window)
	{
		std::vector<ISystem*>& Systems = SystemStorage->SystemsWorkOrder;

		//TODO: Fixed time stamp
		//
		for (ISystem* System : Systems)
		{
			System->FixedTick();
		}
		//

		for (ISystem* System : Systems)
		{
			System->Tick(DeltaTime);
		}

		for (ISystem* System : Systems)
		{
			System->LateTick(DeltaTime);
		}

		Renderer->ClearBackground();

		Window->SwapBuffers();
		Window->PollEvents();

		//OnDrawGizmos...

		//OnGUI...
	}
}
