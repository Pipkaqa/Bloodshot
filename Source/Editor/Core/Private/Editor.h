#pragma once

#include <Core.h>

#include <ComponentManager.h>
#include <EntityManager.h>
#include <Input/Input.h>
#include <Systems/NetworkingSystem.h>
#include <Project.h>
#include <Renderer.h>
#include <Systems/RenderingSystem.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SystemManager.h>
#include <Window.h>

namespace Bloodshot::Editor::Private
{
	class FEditor final
	{
	public:
		FEditor(const std::filesystem::path& ProjectPath);
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
		FApplication Application;
		FInput Input = FInput(Window.GetReference());
		Bloodshot::Private::FRenderingSystem RenderingSystem;
#ifdef BS_NETWORKING_ON
		Bloodshot::Networking::Private::FNetworkingSystem NetworkingSystem;
#endif
		FProject CurrentProject;

		bool bSimulationStartedNow = false;
		bool bSimulating = false;
		bool bSimulationEndedNow = false;

		void Init();
		void Dispose();
		void RunLoop();

		void BeginPlay();
		void EndPlay();
	};
}
