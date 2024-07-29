#include "Core.h"

#include <Benchmark/Profiler.h>

namespace Bloodshot
{
	Core::Core(const ResourceManager::Config& resourceManagerConfig,
		const MemoryManager::Config& memoryManagerConfig,
		const Renderer::Config& rendererConfig,
		const Window::Config& windowConfig,
		const ECS::Config& ecsConfig)
		: m_ResourceManager(ResourceManager::Create(resourceManagerConfig)),
		m_MemoryManager(MemoryManager::Create(memoryManagerConfig)),
		m_Renderer(Renderer::Create(rendererConfig)),
		m_Window(Window::Create(windowConfig))
	{
		s_Instance = this;

		m_ECS = CreateUniquePointer(ECS::Create(ecsConfig, m_EntityManager.get(), m_ComponentManager.get(), m_SystemManager.get()));

		Init();
	}

	Core::~Core()
	{
		BS_DEBUG("Destroying core...");

		Dispose();
	}

	Core* Core::Create(const ResourceManager::Config& resourceManagerConfig,
		const MemoryManager::Config& memoryManagerConfig,
		const Renderer::Config& rendererConfig,
		const Window::Config& windowConfig,
		const ECS::Config& ecsConfig)
	{
		BS_ASSERT(!s_Instance, "An attempt to create another engine instance");

#ifdef BS_LOGGING_ON
		BS_ASSERT(Logger::SessionStarted(), "Log-session not started!");
#endif
#ifdef BS_PROFILING_ON
		BS_ASSERT(Profiler::SessionStarted(), "Profile-session not started!");
#endif

		BS_DEBUG("Creating core...");

		return new Core(resourceManagerConfig, memoryManagerConfig, rendererConfig, windowConfig, ecsConfig);
	}

	void Core::Run()
	{
		s_Instance->StartMainLoop();
	}

	void Core::StartMainLoop()
	{
		BeginSimulation();

		float frameTimeAccumulation = 0.f;

		while (!m_Window->ShouldClose())
		{
			BS_PROFILE_RANGE("Main Loop");

			m_Time->Tick();

			float frameTime = Window::GetFrameTime();

			m_Window->BeginFrame();
			m_SceneManager->InternalUpdate(frameTime, m_Renderer.get(), m_Window.get());
			m_Window->EndFrame();

			frameTimeAccumulation += frameTime;

			if (frameTimeAccumulation >= 1000.f)
			{
				BS_INFO("Global time: {0} s", EngineTime::Global());
				BS_INFO("Frame time: {0} ms", frameTime);
				BS_INFO("FPS: {0}", Window::GetFramesPerSecond());

				frameTimeAccumulation = 0;
			}
		}

		EndSimulation();
	}

	void Core::Init()
	{
		BS_PROFILE_RANGE("Init");

		m_State->Init();
		m_Time->Init();
		m_ResourceManager->Init();
		m_MemoryManager->Init();
		m_Window->Init();
		m_Renderer->Init();
		m_EntityManager->Init();
		m_ComponentManager->Init();
		m_SystemManager->Init();
		m_SceneManager->Init();

		m_State->m_Running = true;
	}

	void Core::Dispose()
	{
		BS_PROFILE_RANGE("Dispose");

		m_SceneManager->Dispose();
		m_SystemManager->Dispose();
		m_ComponentManager->Dispose();
		m_EntityManager->Dispose();
		m_Renderer->Dispose();
		m_Window->Dispose();
		m_MemoryManager->Dispose();
		m_ResourceManager->Dispose();
		m_Time->Dispose();
		m_State->Dispose();

		m_State->m_Running = false;
	}

	void Core::BeginSimulation()
	{
		BS_PROFILE_RANGE("Startup");

		BS_DEBUG("Starting simulation...");

		m_SceneManager->BeginSimulation();

		m_State->m_Simulating = true;

		m_SceneManager->InternalBeginPlay();
	}

	void Core::EndSimulation()
	{
		BS_PROFILE_RANGE("Shutdown");

		BS_DEBUG("Ending simulation...");

		m_SceneManager->InternalEndPlay();

		m_SceneManager->EndSimulation();

		m_State->m_Simulating = false;
	}
}
