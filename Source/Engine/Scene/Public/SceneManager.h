#pragma once

#include "AssertionMacros.h"
#include "EngineState.h"
#include "Logging/LoggingMacros.h"
#include "Platform.h"
#include "Scene.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"

#include <unordered_map>

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class FSceneManager final : public TSingleton<FSceneManager>
	{
		friend int ::main(int Argc, char** Argv);

	public:
		using FTypeIDSceneUnorderedMap = std::unordered_map<TypeID_t, IScene*>;

		FSceneManager();

		FTypeIDSceneUnorderedMap ScenesUnorderedMap;

		IScene* CurrentScenePtr = nullptr;

		virtual void Init() override;
		virtual void Dispose() override;

		NODISCARD FORCEINLINE static IScene* GetCurrentScene()
		{
			return Instance->CurrentScenePtr;
		}

		template<typename T, typename... ArgTypes>
			requires(std::is_base_of_v<IScene, T>)
		static void LoadScene()
		{
			IScene* CurrentScene = Instance->CurrentScenePtr;

			if (CurrentScene)
			{
				BS_LOG(Debug, "End playing on Scene of type: {0}...", CurrentScene->GetTypeName());

				CurrentScene->EndPlay();
				CurrentScene->InternalEndPlay();
			}

			CurrentScene = Instance->ScenesUnorderedMap[TTypeInfo<IScene>::GetTypeID()];

			BS_LOG(Debug, "Begin playing on Scene of type: {0}...", CurrentScene->GetTypeName());

			CurrentScene->InternalBeginPlay();
			CurrentScene->BeginPlay();
		}

		void BeginSimulation();
		void EndSimulation();

		void InternalBeginPlay();
		void InternalEndPlay();

		void InternalUpdate(float DeltaTime, TUniquePtr<IRenderer>& Renderer, TUniquePtr<IWindow>& Window);

	private:
		template<typename T, typename... ArgTypes>
			requires(std::is_base_of_v<IScene, T>)
		static void AddScene(ArgTypes&&... Args)
		{
			BS_ASSERT(!FEngineState::IsSimulating(), "Attempting to create Scene in runtime");

			BS_LOG(Debug, "Creating Scene of type: {0}...", TTypeInfo<T>::GetTypeName());

			const TypeID_t SceneTypeID = TTypeInfo<IScene>::GetTypeID();

			FTypeIDSceneUnorderedMap& Scenes = Instance->ScenesUnorderedMap;

			FTypeIDSceneUnorderedMap::iterator It = Scenes.find(SceneTypeID);

			BS_ASSERT(It == Scenes.end() || !It->second, "Attempting to create already existing Scene");

			It = Scenes.emplace(std::make_pair(SceneTypeID, new T(std::forward<ArgTypes>(Args)...))).first;

			It->second->UniqueID = SceneTypeID;
		}

		template<typename T, typename... ArgTypes>
			requires(std::is_base_of_v<IScene, T>)
		static void SetStartingScene()
		{
			BS_ASSERT(!FEngineState::IsSimulating(), "Attempting to set starting Scene in runtime");

			const TypeID_t SceneTypeID = TTypeInfo<IScene>::GetTypeID();

			const FTypeIDSceneUnorderedMap& Scenes = Instance->ScenesUnorderedMap;

			const FTypeIDSceneUnorderedMap::const_iterator& It = Scenes.find(SceneTypeID);

			BS_ASSERT(It != Scenes.end() && It->second, "Attempting to set not existing Scene as starting");

			Instance->CurrentScenePtr = It->second;
		}
	};
}
