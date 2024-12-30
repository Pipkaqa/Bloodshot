#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	class IECS abstract final
	{
		friend class FScene;

	public:
		static inline size_t EntityStorageGrow = 1024;
		static inline size_t ComponentStorageGrow = 4096;
		static inline size_t SystemStorageGrow = 64;

		static TReference<FEntity> Instantiate();
		static void InstantiateMultiple(const size_t Count);
		static void InstantiateMultiple(std::vector<TReference<FEntity>>& OutResult, const size_t Count);

		template<size_t Count>
		static void InstantiateMultiple(std::array<TReference<FEntity>, Count>& OutResult)
		{
			BS_PROFILE_FUNCTION();

			for (TReference<FEntity>& Entity : OutResult)
			{
				Entity = FEntityManager::Instantiate();
			}
		}

		static void Destroy(TReference<FEntity> Entity);
		static void DestroyMultiple(std::vector<TReference<FEntity>>& OutEntities);

		template<CIsComponent T, typename... ArgTypes>
		static TReference<T> AddComponent(TReference<FEntity> Entity, ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			TReference<T> Component = FComponentManager::AddComponent<T>(Entity, std::forward<ArgTypes>(Args)...);

			if constexpr (std::is_same_v<FCameraComponent, T>)
			{
				TReference<FScene> CurrentScene = FSceneManager::Instance->CurrentScene;

				CurrentScene->CameraVec.push_back(Component);

				TReference<FCameraComponent>& MainCameraComponent = CurrentScene->MainCameraComponent;

				if (!MainCameraComponent) MainCameraComponent = Component;
			}

			return Component;
		}

		template<CIsComponent T>
		static void RemoveComponent(TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			FComponentManager::RemoveComponent<T>(Entity);
		}

		static void RemoveAllComponents(TReference<FEntity> Entity);

		template<CIsComponent T>
		NODISCARD static TReference<T> GetComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::GetComponent<T>(Entity);
		}

		template<CIsComponent T>
		NODISCARD static bool HasComponent(const TReference<FEntity> Entity)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::HasComponent<T>(Entity);
		}

		template<CIsComponent T>
		NODISCARD FORCEINLINE static TComponentIterator<T> GetComponentBeginIterator()
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::Begin<T>();
		}

		template<CIsComponent T>
		NODISCARD FORCEINLINE static TComponentIterator<T> GetComponentEndIterator()
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::End<T>();
		}

		template<CIsSystem T, typename... ArgTypes>
		static TReference<T> AddSystem(ArgTypes&&... args)
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
		NODISCARD static TReference<T> GetSystem()
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

	private:
		static void InternalRemoveAllComponents();
		static void InternalDestroyAllEntities();
	};
}
