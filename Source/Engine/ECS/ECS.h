#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Scene/Scene.h"
#include "SystemManager.h"
#include "Utility/ISingleton.h"
#include "Utility/Utility.h"

namespace Bloodshot
{
	class ECS final : public ISingleton<ECS>
	{
		OWNED_BY_CORE;
		ECS_PART;

	public:
		struct Config final
		{
			const size_t m_EntitiesStorageGrow = 1024;
			const size_t m_ComponentsStorageGrow = 4096;
		};

		NODISCARD FORCEINLINE static const Config& GetConfig()
		{
			return *s_Instance->m_Config;
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IEntity, T>
		static T* Instantiate(Args&&... args)
		{
			return s_Instance->m_EntityManager->Instantiate<T>(GetCurrentScene()->m_EntityStorage.get(), std::forward<Args>(args)...);
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IEntity, T>
		static std::vector<T*> InstantiateMultiple(const size_t count, Args&&... args)
		{
			std::vector<T*> result(count);

			auto entityStorage = GetCurrentScene()->m_EntityStorage.get();

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
			s_Instance->RemoveAllComponents(entityInterface);
			s_Instance->m_EntityManager->Destroy(GetCurrentScene()->m_EntityStorage.get(), entityInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		static void DestroyMultiple(T** entityInterfaces, const size_t count)
		{
			auto entityStorage = GetCurrentScene()->m_EntityStorage.get();

			for (int i = 0; i < count; i++)
			{
				auto entityInterface = entityInterfaces[i];

				if (!entityInterface) continue;

				s_Instance->RemoveAllComponents(entityInterface);
				s_Instance->m_EntityManager->Destroy(entityStorage, entityInterface);
			}
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IComponent, T>
		static T* AddComponent(IEntity* entityInterface, Args&&... args)
		{
			return s_Instance->m_ComponentManager->AddComponent<T>(GetCurrentScene()->m_ComponentStorage.get(), entityInterface, std::forward<Args>(args)...);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		static void RemoveComponent(const IEntity* entityInterface)
		{
			//TODO: optimize, do raw get and minimize func calls
			s_Instance->m_ComponentManager->RemoveComponent<T>(GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
		}

		static void RemoveAllComponents(const IEntity* entityInterface);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static T* GetComponent(const IEntity* entityInterface)
		{
			return s_Instance->m_ComponentManager->GetComponent<T>(GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD static bool HasComponent(const IEntity* entityInterface)
		{
			return s_Instance->m_ComponentManager->HasComponent<T>(GetCurrentScene()->m_ComponentStorage.get(), entityInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD FORCEINLINE static ComponentManager::ComponentIterator<T> Begin()
		{
			return s_Instance->m_ComponentManager->Begin<T>();
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD FORCEINLINE static ComponentManager::ComponentIterator<T> End()
		{
			return s_Instance->m_ComponentManager->End<T>();
		}

		template<typename T, typename... Args>
			requires (std::is_base_of_v<ISystem, T>)
		static T* AddSystem(Args&&... args)
		{
			return s_Instance->m_SystemManager->AddSystem<T>(GetCurrentScene()->m_SystemStorage.get(), std::forward<Args>(args)...);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		NODISCARD static T* GetSystem()
		{
			return s_Instance->m_SystemManager->GetSystem<T>(GetCurrentScene()->m_SystemStorage.get());
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void EnableSystem()
		{
			return s_Instance->m_SystemManager->EnableSystem<T>(GetCurrentScene()->m_SystemStorage.get());
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void DisableSystem()
		{
			return s_Instance->m_SystemManager->DisableSystem<T>(GetCurrentScene()->m_SystemStorage.get());
		}

	private:
		using ISingleton::Create;

		const Config* m_Config = nullptr;

		EntityManager* m_EntityManager = nullptr;
		ComponentManager* m_ComponentManager = nullptr;
		SystemManager* m_SystemManager = nullptr;

		NODISCARD static ECS* Create(const Config& config, EntityManager* entityManager, ComponentManager* componentManager, SystemManager* systemManager);

		void Init() override;
		void Dispose() override;

		static Scene* GetCurrentScene();
	};
}
