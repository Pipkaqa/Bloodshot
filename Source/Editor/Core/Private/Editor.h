#pragma once

#include <Application.h>
#include <ComponentManager.h>
#include <EngineState.h>
#include <EngineTime.h>
#include <EntityManager.h>
#include <Input.h>
#include <Logging/Logger.h>
#include <Memory/Memory.h>
#include <NetworkingSystem.h>
#include <Profiling/Profiler.h>
#include <Renderer.h>
#include <RenderingSystem.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SystemManager.h>
#include <Templates/SmartPointers.h>
#include <Window.h>

namespace Bloodshot::Editor::Private
{
	class FEditor final
	{
	public:
		FEditor();
		~FEditor();

	private:
		struct FSettings final
		{
			ERendererType RendererType = ERendererType::OpenGL;
		};

		FSettings Settings;

		FMemory Memory;
		FEngineState EngineState;
		FEngineTime EngineTime;
		FResourceManager ResourceManager;
		TUniquePtr<IWindow> Window = nullptr;
		TUniquePtr<IRenderer> Renderer = nullptr;
		FEntityManager EntityManager;
		FComponentManager ComponentManager;
		FSystemManager SystemManager;
		FSceneManager SceneManager;
		FApplication Application = {Window};
		FInput Input = {Window};
		Bloodshot::Private::FRenderingSystem RenderingSystem;
#ifdef BS_NETWORKING_ON
		Bloodshot::Networking::Private::FNetworkingSystem NetworkingSystem;
#endif
		void Init();
		void Dispose();
		void Run();

		void BeginPlay();
		void EndPlay();

		void SetTestScene();
	};
}
