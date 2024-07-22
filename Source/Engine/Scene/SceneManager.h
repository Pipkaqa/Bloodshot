#pragma once

#include "Core/Assert.h"
#include "Core/EngineState.h"
#include "Memory/LinearAllocator.h"
#include "Debug/Logger.h"
#include "Scene.h"
#include "Utility/TypeInfo.h"

#include <vector>

namespace Bloodshot
{
	class Renderer;

	class SceneManager final : public ISingleton<SceneManager>
	{
		OWNED_BY_CORE;

	public:
		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void AddScene(Args&&... args)
		{
			FL_CORE_ASSERT(!EngineState::Simulating(), "An attempt to add scene in runtime");

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			auto& scenes = s_Instance->m_Scenes;

			FL_CORE_ASSERT(s_Instance->m_Scenes.find(sceneTypeID) == s_Instance->m_Scenes.end() || !s_Instance->m_Scenes[sceneTypeID], "An attempt to add scene that already exists");

			auto& allocator = s_Instance->m_Allocator;

			auto* memory = allocator->Allocate(sizeof(T));

			Scene* scene = new(memory) T(std::forward<Args>(args)...);

			scene->m_UniqueID = sceneTypeID;

			s_Instance->m_Scenes[sceneTypeID] = scene;
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void SetStartingScene()
		{
			FL_CORE_ASSERT(!EngineState::Simulating(), "An attempt to set active scene in runtime");

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			FL_CORE_ASSERT(s_Instance->m_Scenes.find(sceneTypeID) != s_Instance->m_Scenes.end() && s_Instance->m_Scenes[sceneTypeID], "An attempt to set scene as active that not exists");

			s_Instance->m_ActiveScene = s_Instance->m_Scenes[sceneTypeID];
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void LoadScene()
		{
			auto& activeScene = s_Instance->m_ActiveScene;

			if (activeScene)
			{
				FL_CORE_DEBUG("Destroying scene of type[{0}]...", activeScene->GetTypeName());

				activeScene->EndPlay();
				activeScene->InternalEndPlay();
			}

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			activeScene = s_Instance->m_Scenes[sceneTypeID];

			FL_CORE_DEBUG("Creating scene of type[{0}]...", activeScene->GetTypeName());

			activeScene->InternalBeginPlay();
			activeScene->BeginPlay();
		}

	private:
		UniquePointer<LinearAllocator> m_Allocator = CreateUniquePointer<LinearAllocator>();

		Scene* m_ActiveScene = nullptr;

		std::unordered_map<SceneTypeID, Scene*> m_Scenes;

		void Init() override;
		void Dispose() override;

		void BeginSimulation();
		void EndSimulation();

		void BeginPlay();
		void EndPlay();

		void InternalUpdate(float deltaTime, Renderer* renderer);

		friend class ECS;
	};
}
