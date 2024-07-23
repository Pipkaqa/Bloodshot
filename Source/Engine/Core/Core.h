#pragma once

#include "ECS/ECS.h"
#include "EngineState.h"
#include "Memory/MemoryManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/Window.h"
#include "ResourceManager.h"
#include "Scene/SceneManager.h"
#include "Utility/Utility.h"

int main(int argc, char** argv);

namespace Bloodshot
{
	class Core final : public ISingleton<Core>
	{
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

		UniquePointer<ResourceManager> m_ResourceManager;
		UniquePointer<MemoryManager> m_MemoryManager;
		UniquePointer<Renderer> m_Renderer;
		UniquePointer<Window> m_Window;
		UniquePointer<ECS> m_ECS;

		UniquePointer<EntityManager> m_EntityManager = CreateUniquePointer(EntityManager::Create());
		UniquePointer<ComponentManager> m_ComponentManager = CreateUniquePointer(ComponentManager::Create());
		UniquePointer<SystemManager> m_SystemManager = CreateUniquePointer(SystemManager::Create());
		UniquePointer<SceneManager> m_SceneManager = CreateUniquePointer(SceneManager::Create());

		static void Create(const ResourceManager::Config& resourceManagerConfig = {},
			const MemoryManager::Config& memoryManagerConfig = {},
			const Renderer::Config& rendererConfig = {},
			const Window::Config& windowConfig = {},
			const ECS::Config& ecsConfig = {});

		static void Run();

		void Init() override;
		void Dispose() override;

		void BeginSimulation();
		void EndSimulation();

		friend int ::main(int argc, char** argv);
	};
}
