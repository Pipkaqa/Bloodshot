#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Scene/SceneManager.h"
#include "SystemManager.h"

namespace Bloodshot
{
	class ECS final : public IStaticSingleton<ECS>
	{
		CORE_MODULE;
		ECS_MODULE;

	public:
		struct Config final
		{
			size_t m_EntitiesStorageGrow = 1024;
			size_t m_ComponentsStorageGrow = 4096;
		};

		NODISCARD FORCEINLINE static const Config& GetConfig()
		{
			return s_Instance->m_Config;
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IEntity, T>
		static T* Instantiate(Args&&... args)
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_EntityManager->Instantiate<T>(SceneManager::GetCurrentScene()->m_EntityStorage.get(), std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IEntity, T>
		static std::vector<T*> InstantiateMultiple(const size_t count, Args&&... args)
		{
			BS_PROFILE_FUNCTION();

			std::vector<T*> result(count);

			auto entityStorage = SceneManager::GetCurrentScene()->m_EntityStorage.get();

			for (auto& entity : result)
			{
				entity = s_Instance->m_EntityManager->Instantiate<T>(entityStorage, std::forward<Args>(args)...);
			}

			return result;
		}

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		static void Destroy(T* entityInterface)
		{
			BS_PROFILE_FUNCTION();

			auto currentScene = SceneManager::GetCurrentScene();

			s_Instance->m_ComponentManager->RemoveAllComponents(currentScene->m_ComponentStorage.get(), entityInterface);
			s_Instance->m_EntityManager->Destroy(currentScene->m_EntityStorage.get(), entityInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		static void DestroyMultiple(T** entityInterfaces, const size_t count)
		{
			BS_PROFILE_FUNCTION();

			auto currentScene = SceneManager::GetCurrentScene();
			auto entityStorage = currentScene->m_EntityStorage.get();
			auto componentStorage = currentScene->m_ComponentStorage.get();

			for (int i = 0; i < count; i++)
			{
				auto entityInterface = entityInterfaces[i];

				if (!entityInterface) continue;

				s_Instance->m_ComponentManager->RemoveAllComponents(componentStorage, entityInterface);
				s_Instance->m_EntityManager->Destroy(entityStorage, entityInterface);
			}
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IComponent, T>
		static T* AddComponent(IEntity* entityInterface, Args&&... args)
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_ComponentManager->AddComponent<T>(SceneManager::GetCurrentScene()->m_ComponentStorage.get(), entityInterface, std::forward<Args>(args)...);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		static void RemoveComponent(IEntity* entityInterface)
		{
			BS_PROFILE_FUNCTION();

			s_Instance->m_ComponentManager->RemoveComponent<T>(SceneManager::GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
		}

		static void RemoveAllComponents(IEntity* entityInterface);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static T* GetComponent(const IEntity* entityInterface)
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_ComponentManager->GetComponent<T>(SceneManager::GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static bool HasComponent(const IEntity* entityInterface)
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_ComponentManager->HasComponent<T>(SceneManager::GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD FORCEINLINE static ComponentManager::ComponentIterator<T> Begin()
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_ComponentManager->Begin<T>();
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD FORCEINLINE static ComponentManager::ComponentIterator<T> End()
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_ComponentManager->End<T>();
		}

		template<typename T, typename... Args>
			requires (std::is_base_of_v<ISystem, T>)
		static T* AddSystem(Args&&... args)
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_SystemManager->AddSystem<T>(SceneManager::GetCurrentScene()->m_SystemStorage.get(), std::forward<Args>(args)...);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();

			s_Instance->m_SystemManager->RemoveSystem<T>(SceneManager::GetCurrentScene()->m_SystemStorage.get());
		}

		static void RemoveAllSystems();

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		NODISCARD static T* GetSystem()
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_SystemManager->GetSystem<T>(SceneManager::GetCurrentScene()->m_SystemStorage.get());
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_SystemManager->EnableSystem<T>(SceneManager::GetCurrentScene()->m_SystemStorage.get());
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			return s_Instance->m_SystemManager->DisableSystem<T>(SceneManager::GetCurrentScene()->m_SystemStorage.get());
		}

	private:
		using IStaticSingleton::Create;

		Config m_Config = {};

		EntityManager* m_EntityManager = nullptr;
		ComponentManager* m_ComponentManager = nullptr;
		SystemManager* m_SystemManager = nullptr;

		NODISCARD static ECS* Create(const Config& config, EntityManager* entityManager, ComponentManager* componentManager, SystemManager* systemManager);
	};
}
