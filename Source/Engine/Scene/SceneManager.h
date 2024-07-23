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
	class Window;

	class SceneManager final : public ISingleton<SceneManager>
	{
		OWNED_BY_CORE;
		ECS_PART;

	public:
		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void AddScene(Args&&... args)
		{
			FL_CORE_ASSERT(!EngineState::Simulating(), "An attempt to create scene in runtime");

			FL_CORE_DEBUG("Creating scene of type [{0}]...", TypeInfo<T>::GetTypeName());

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			auto& scenes = s_Instance->m_Scenes;

			FL_CORE_ASSERT(s_Instance->m_Scenes.find(sceneTypeID) == s_Instance->m_Scenes.end() || !s_Instance->m_Scenes[sceneTypeID], "An attempt to create scene that already exists");

			Scene* scene = new T(std::forward<Args>(args)...);

			scene->m_UniqueID = sceneTypeID;

			s_Instance->m_Scenes[sceneTypeID] = scene;
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void SetStartingScene()
		{
			FL_CORE_ASSERT(!EngineState::Simulating(), "An attempt to set starting scene in runtime");

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			FL_CORE_ASSERT(s_Instance->m_Scenes.find(sceneTypeID) != s_Instance->m_Scenes.end() && s_Instance->m_Scenes[sceneTypeID], "An attempt to set scene as starting that not exists");

			s_Instance->m_ActiveScene = s_Instance->m_Scenes[sceneTypeID];
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void LoadScene()
		{
			auto& activeScene = s_Instance->m_ActiveScene;

			if (activeScene)
			{
				FL_CORE_DEBUG("End playing on scene of type [{0}]...", activeScene->GetTypeName());

				activeScene->EndPlay();
				activeScene->InternalEndPlay();
			}

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			activeScene = s_Instance->m_Scenes[sceneTypeID];

			FL_CORE_DEBUG("Begin playing on scene of type [{0}]...", activeScene->GetTypeName());

			activeScene->InternalBeginPlay();
			activeScene->BeginPlay();
		}

	private:
		using ISingleton::Create;

		Scene* m_ActiveScene = nullptr;

		std::unordered_map<SceneTypeID, Scene*> m_Scenes;

		void Init() override;
		void Dispose() override;

		void BeginSimulation();
		void EndSimulation();

		void InternalBeginPlay();
		void InternalEndPlay();

		void InternalUpdate(float deltaTime, Renderer* renderer, Window* window);
	};
}
