#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"
#include "SceneManager.h"
#include "SystemManager.h"

namespace Bloodshot
{
	namespace
	{
		static IScene* GetCurrentScene()
		{
			return FSceneManager::GetCurrentScene();
		}
	}

	struct IECS abstract final
	{
		static inline size_t EntityStorageGrow = 1024;
		static inline size_t ComponentStorageGrow = 4096;

		template<typename T, typename... ArgTypes>
			requires std::is_base_of_v<IEntity, T>
		static T* Instantiate(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			return FEntityManager::Instantiate<T>(GetCurrentScene()->EntityStorage, std::forward<ArgTypes>(Args)...);
		}

		template<typename T, typename... ArgTypes>
			requires std::is_base_of_v<IEntity, T>
		static void InstantiateMultiple(const size_t Count, ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			TUniquePtr<FEntityStorage>& EntityStorage = GetCurrentScene()->EntityStorage;

			for (size_t i = 0U; i < Count; ++i)
			{
				FEntityManager::Instantiate<T>(EntityStorage, std::forward<ArgTypes>(Args)...);
			}
		}

		template<typename T, typename... ArgTypes>
			requires std::is_base_of_v<IEntity, T>
		static void InstantiateMultiple(const size_t Count, std::vector<T*>& OutResult, ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			OutResult.resize(Count);

			TUniquePtr<FEntityStorage>& EntityStorage = GetCurrentScene()->EntityStorage;

			for (T*& Entity : OutResult)
			{
				Entity = FEntityManager::Instantiate<T>(EntityStorage, std::forward<ArgTypes>(Args)...);
			}
		}

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		static void Destroy(T* const Entity)
		{
			BS_PROFILE_FUNCTION();

			IScene* CurrentScene = GetCurrentScene();

			FComponentManager::RemoveAllComponents(CurrentScene->ComponentStorage, Entity);
			FEntityManager::Destroy(CurrentScene->EntityStorage, Entity);
		}

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		static void DestroyMultiple(std::vector<T*>& OutEntities)
		{
			BS_PROFILE_FUNCTION();

			IScene* const CurrentScene = GetCurrentScene();

			TUniquePtr<FEntityStorage>& EntityStorage = CurrentScene->EntityStorage;
			TUniquePtr<FComponentStorage>& ComponentStorage = CurrentScene->ComponentStorage;

			for (T* const Entity : OutEntities)
			{
				if (!Entity) continue;

				FComponentManager::RemoveAllComponents(ComponentStorage, Entity);
				FEntityManager::Destroy(EntityStorage, Entity);
			}

			OutEntities.clear();
		}

		template<typename T, typename... ArgTypes>
			requires std::is_base_of_v<IComponent, T>
		static T* AddComponent(IEntity* const Entity, ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::AddComponent<T>(GetCurrentScene()->ComponentStorage, Entity, std::forward<ArgTypes>(Args)...);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		static void RemoveComponent(IEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();

			FComponentManager::RemoveComponent<T>(GetCurrentScene()->ComponentStorage, Entity);
		}

		static void RemoveAllComponents(IEntity* const Entity);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static T* GetComponent(const IEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::GetComponent<T>(GetCurrentScene()->ComponentStorage, Entity);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static bool HasComponent(const IEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::HasComponent<T>(GetCurrentScene()->ComponentStorage, Entity);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD FORCEINLINE static TComponentIterator<T> GetBeginComponentIterator()
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::Begin<T>();
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD FORCEINLINE static TComponentIterator<T> GetEndComponentIterator()
		{
			BS_PROFILE_FUNCTION();

			return FComponentManager::End<T>();
		}

		template<typename T, typename... ArgTypes>
			requires (std::is_base_of_v<ISystem, T>)
		static T* AddSystem(ArgTypes&&... args)
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::AddSystem<T>(GetCurrentScene()->SystemStorage, std::forward<ArgTypes>(args)...);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();

			FSystemManager::RemoveSystem<T>(GetCurrentScene()->SystemStorage);
		}

		static void RemoveAllSystems();

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		NODISCARD static T* GetSystem()
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::GetSystem<T>(GetCurrentScene()->SystemStorage);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::EnableSystem<T>(GetCurrentScene()->SystemStorage);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			return FSystemManager::DisableSystem<T>(GetCurrentScene()->SystemStorage);
		}
	};
}
