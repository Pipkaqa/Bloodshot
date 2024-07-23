#include "Core.h"

#include "Benchmark/Timer.h"

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
		m_ECS = CreateUniquePointer(ECS::Create(ecsConfig, m_EntityManager.get(), m_ComponentManager.get(), m_SystemManager.get()));

		Init();
	}

	Core::~Core()
	{
		Dispose();
	}

	void Core::Create(const ResourceManager::Config& resourceManagerConfig,
		const MemoryManager::Config& memoryManagerConfig,
		const Renderer::Config& rendererConfig,
		const Window::Config& windowConfig,
		const ECS::Config& ecsConfig)
	{
		FL_CORE_ASSERT(!s_Instance, "An attempt to create another engine instance");

		FL_CORE_DEBUG("Creating core...");

		s_Instance = new Core(resourceManagerConfig, memoryManagerConfig, rendererConfig, windowConfig, ecsConfig);
	}

	void Core::Run()
	{
		auto& instance = Core::s_Instance;

		instance->BeginSimulation();

		Timer timer = {};
		std::chrono::microseconds deltaTime = {};

		while (!instance->m_Window->ShouldClose())
		{
			timer.Reset();

			//Raylib::DrawText(Raylib::TextFormat("FPS: %i", Raylib::GetFPS()), 5, 5, 20, Raylib::LIME);
			//Raylib::DrawText(Raylib::TextFormat("Frame time: %i us", deltaTime.count()), 5, 25, 20, Raylib::LIME);
			//Raylib::DrawText(Raylib::TextFormat("Used VRAM: %i.MB", MemoryManager::GetCurrentMemorySizeUsage() / 1024 / 1024), 5, 45, 20, Raylib::LIME);

			instance->m_SceneManager->InternalUpdate(deltaTime.count(), instance->m_Renderer.get(), instance->m_Window.get());
			deltaTime = timer.GetElapsedMicroseconds();
		}

		instance->EndSimulation();

		delete s_Instance;
	}

	void Core::Init()
	{
		m_ResourceManager->Init();
		m_MemoryManager->Init();
		m_Renderer->Init();
		m_Window->Init();
		m_EntityManager->Init();
		m_ComponentManager->Init();
		m_SystemManager->Init();
		m_SceneManager->Init();
		m_ECS->Init();

		m_State->m_Running = true;
	}

	void Core::Dispose()
	{
		FL_CORE_DEBUG("Destroying core...");

		m_ECS->Dispose();
		m_SceneManager->Dispose();
		m_SystemManager->Dispose();
		m_ComponentManager->Dispose();
		m_EntityManager->Dispose();
		m_Window->Dispose();
		m_Renderer->Dispose();
		m_MemoryManager->Dispose();
		m_ResourceManager->Dispose();

		m_State->m_Running = false;
	}

	void Core::BeginSimulation()
	{
		FL_CORE_DEBUG("Starting simulation...");

		m_SceneManager->BeginSimulation();

		m_State->m_Simulating = true;

		m_SceneManager->InternalBeginPlay();
	}

	void Core::EndSimulation()
	{
		FL_CORE_DEBUG("Ending simulation...");

		m_SceneManager->InternalEndPlay();

		m_SceneManager->EndSimulation();

		m_State->m_Simulating = false;
	}
}
