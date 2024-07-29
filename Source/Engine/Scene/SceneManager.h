#pragma once

#include "Core/EngineState.h"
#include "Debug/Logger.h"
#include "Scene.h"

namespace Bloodshot
{
	class Renderer;
	class Window;

	class SceneManager final : public ISingleton<SceneManager>
	{
		CORE_MODULE;
		ECS_MODULE;

	public:
		NODISCARD FORCEINLINE static Scene* GetCurrentScene()
		{
			return s_Instance->m_CurrentScene;
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void AddScene(Args&&... args)
		{
			BS_ASSERT(!EngineState::Simulating(), "An attempt to create scene in runtime");

			BS_DEBUG("Creating scene of type [{0}]...", TypeInfo<T>::GetTypeName());

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			auto& scenes = s_Instance->m_Scenes;

			BS_ASSERT(s_Instance->m_Scenes.find(sceneTypeID) == s_Instance->m_Scenes.end()
				|| !s_Instance->m_Scenes[sceneTypeID],
				"An attempt to create already existing scene");

			Scene* scene = new T(std::forward<Args>(args)...);

			scene->m_UniqueID = sceneTypeID;

			s_Instance->m_Scenes[sceneTypeID] = scene;
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void SetStartingScene()
		{
			BS_ASSERT(!EngineState::Simulating(), "An attempt to set starting scene in runtime");

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			BS_ASSERT(s_Instance->m_Scenes.find(sceneTypeID) != s_Instance->m_Scenes.end()
				&& s_Instance->m_Scenes[sceneTypeID],
				"An attempt to set not existing scene as starting");

			s_Instance->m_CurrentScene = s_Instance->m_Scenes[sceneTypeID];
		}

		template<typename T, typename... Args>
			requires(std::is_base_of_v<Scene, T>)
		static void LoadScene()
		{
			auto& currentScene = s_Instance->m_CurrentScene;

			if (currentScene)
			{
				BS_DEBUG("End playing on scene of type [{0}]...", currentScene->GetTypeName());

				currentScene->EndPlay();
				currentScene->InternalEndPlay();
			}

			const auto sceneTypeID = TypeInfo<Scene>::GetTypeID<T>();

			currentScene = s_Instance->m_Scenes[sceneTypeID];

			BS_DEBUG("Begin playing on scene of type [{0}]...", currentScene->GetTypeName());

			currentScene->InternalBeginPlay();
			currentScene->BeginPlay();
		}

	private:
		using ISingleton::Create;

		Scene* m_CurrentScene = nullptr;
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
