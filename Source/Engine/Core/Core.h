#pragma once

#include "ECS/ECS.h"
#include "Rendering/Renderer.h"
#include "Rendering/Window.h"
#include "ResourceManager.h"

namespace Bloodshot
{
	class Core final : public ISingleton<Core>
	{
		ENTRYPOINT;

	public:
		~Core();

	private:
		using ISingleton::Create;

		Core(const ResourceManager::Config& resourceManagerConfig,
			const MemoryManager::Config& memoryManagerConfig,
			const Renderer::Config& rendererConfig,
			const Window::Config& windowConfig,
			const ECS::Config& ecsConfig);

		UniquePointer<EngineState> m_State = CreateUniquePointer(EngineState::Create());
		UniquePointer<EngineTime> m_Time = CreateUniquePointer(EngineTime::Create());

		UniquePointer<ResourceManager> m_ResourceManager;
		UniquePointer<MemoryManager> m_MemoryManager;
		UniquePointer<Renderer> m_Renderer;
		UniquePointer<Window> m_Window;
		UniquePointer<ECS> m_ECS;

		UniquePointer<EntityManager> m_EntityManager = CreateUniquePointer(EntityManager::Create());
		UniquePointer<ComponentManager> m_ComponentManager = CreateUniquePointer(ComponentManager::Create());
		UniquePointer<SystemManager> m_SystemManager = CreateUniquePointer(SystemManager::Create());
		UniquePointer<SceneManager> m_SceneManager = CreateUniquePointer(SceneManager::Create());

		NODISCARD static Core* Create(const ResourceManager::Config& resourceManagerConfig = {},
			const MemoryManager::Config& memoryManagerConfig = {},
			const Renderer::Config& rendererConfig = {},
			const Window::Config& windowConfig = {},
			const ECS::Config& ecsConfig = {});

		static void Run();

		void Init() override;
		void Dispose() override;

		void BeginSimulation();
		void EndSimulation();

		void StartMainLoop();
	};
}
