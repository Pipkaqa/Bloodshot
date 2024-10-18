#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SystemManager.h"

namespace Bloodshot
{
	struct IECS abstract final
	{
		static inline size_t EntityStorageGrow = 1024;
		static inline size_t ComponentStorageGrow = 4096;
		static inline size_t SystemStorageGrow = 64;

		static FEntity* Instantiate();
		static void InstantiateMultiple(const size_t Count);
		static void InstantiateMultiple(std::vector<FEntity*>& OutResult, const size_t Count);

		template<size_t Count>
		static void InstantiateMultiple(std::array<FEntity*, Count>& OutResult)
		{
			BS_PROFILE_FUNCTION();

			for (FEntity*& Entity : OutResult)
			{
				Entity = FEntityManager::Instantiate();
			}
		}

		static void Destroy(FEntity* const Entity);
		static void DestroyMultiple(std::vector<FEntity*>& OutEntities);

		template<CIsComponent T, typename... ArgTypes>
		static T* AddComponent(FEntity* const Entity, ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			T* const Component = FComponentManager::AddComponent<T>(Entity, std::forward<ArgTypes>(Args)...);

			if constexpr (std::is_same_v<FCameraComponent, T>)
			{
				FScene* const CurrentScene = FSceneManager::Instance->CurrentScene;

				CurrentScene->CameraVec.push_back(Component);

				FCameraComponent*& MainCameraComponent = CurrentScene->MainCameraComponent;

				if (!MainCameraComponent) MainCameraComponent = Component;
			}

			return Component;
		}

		template<CIsComponent T>
		static void RemoveComponent(FEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();

			FComponentManager::RemoveComponent<T>(Entity);
		}

		static void RemoveAllComponents(FEntity* const Entity);

		template<CIsComponent T>
		NODISCARD static T* GetComponent(const FEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::GetComponent<T>(Entity);
		}

		template<CIsComponent T>
		NODISCARD static bool HasComponent(const FEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::HasComponent<T>(Entity);
		}

		template<CIsComponent T>
		NODISCARD FORCEINLINE static TComponentIterator<T> GetBeginComponentIterator()
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::Begin<T>();
		}

		template<CIsComponent T>
		NODISCARD FORCEINLINE static TComponentIterator<T> GetEndComponentIterator()
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::End<T>();
		}

		template<CIsSystem T, typename... ArgTypes>
		static T* AddSystem(ArgTypes&&... args)
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::AddSystem<T>(std::forward<ArgTypes>(args)...);
		}

		template<CIsSystem T>
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();

			FSystemManager::RemoveSystem<T>();
		}

		static void RemoveAllSystems();

		template<CIsSystem T>
		NODISCARD static T* GetSystem()
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::GetSystem<T>();
		}

		template<CIsSystem T>
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::EnableSystem<T>();
		}

		template<CIsSystem T>
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::DisableSystem<T>();
		}
	};
}
