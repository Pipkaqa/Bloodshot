#include "Scene.h"
#include "ECS.h"
#include "Networking.h"
#include "NetworkingSystem.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "System.h"
#include "Window.h"

namespace Bloodshot
{
	FScene::FScene(const InstanceID_t InstanceID)
		: InstanceID(InstanceID)
	{
	}

	void FScene::BeginPlay()
	{
	}

	void FScene::EndPlay()
	{
		IECS::RemoveAllSystems();
	}

	void FScene::Tick(float DeltaTime, TUniquePtr<IRenderer>& Renderer, TUniquePtr<IWindow>& Window)
	{
		NetworkingSystem.Execute(DeltaTime);

		Window->PollEvents();

		std::vector<ISystem*>& Systems = FSystemManager::GetSystems();

		// BSTODO: Fixed time stamp
		{
			for (ISystem* const System : Systems)
			{
				if (System)
				{
					System->FixedTick();
				}
			}
		}

		for (ISystem* const System : Systems)
		{
			if (System)
			{
				System->Tick(DeltaTime);
			}
		}

		for (ISystem* const System : Systems)
		{
			if (System)
			{
				System->LateTick(DeltaTime);
			}
		}

		Renderer->ClearBackground();

		RenderingSystem.Execute(DeltaTime, Renderer);

		Window->SwapBuffers();

		//OnDrawGizmos...

		//OnGUI...
	}
}
