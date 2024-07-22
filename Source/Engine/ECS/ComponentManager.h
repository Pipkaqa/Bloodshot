#pragma once

#include "ComponentStorage.h"
#include "Debug/Logger.h"
#include "IComponent.h"
#include "IEntity.h"
#include "Memory/FixedAllocator.h"

#include <format>
#include <unordered_map>

namespace Bloodshot
{
	class ComponentManager final : public ISingleton<ComponentManager>
	{
		OWNED_BY_CORE;
		ECS_PART;

		struct IComponentPool abstract
		{
			ECS_PART;

			virtual ~IComponentPool() {}

			NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;
			virtual void Release(IComponent* ptr) = 0;
		};

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		struct ComponentPool final : public IComponentPool, public FixedAllocator<T>
		{
			ECS_PART;

			ComponentPool() {}

			ComponentPool(const ComponentPool& other) = delete;

			~ComponentPool() override
			{
				FixedAllocator<T>::Release();
			}

			ComponentPool& operator=(const ComponentPool& other) = delete;

			NODISCARD FORCEINLINE const char* GetTypeName() const noexcept override
			{
				static const char* typeName{typeid(T).name()};
				return typeName;
			}

			void Release(IComponent* componentInterface) override
			{
				FixedAllocator<T>::Deallocate(componentInterface);
			}
		};

		template<typename T>
		using ComponentIterator = ComponentPool<T>::Iterator;

		std::unordered_map<ComponentTypeID, IComponentPool*> m_ComponentPools;

		template<typename T, typename... Args>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD std::pair<IComponent*, T*> AddComponent(ComponentStorage* storage, IEntity* entityInterface, Args&&... args)
		{
			const auto& componentTypeID = T::s_TypeID;

			const auto memory = GetComponentPool<T>()->Allocate();

			IComponent* componentInterface = new(memory) T(std::forward<Args>(args)...);

			storage->Store(entityInterface, componentInterface, componentTypeID);

			return std::make_pair(componentInterface, FastCast<T*>(componentInterface));
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		void RemoveComponent(ComponentStorage* storage, const IEntity* entityInterface)
		{
			const auto componentTypeID = T::s_TypeID;

			const auto entityID = entityInterface->m_UniqueID;

			const auto componentID = storage->m_ComponentMap[entityID][componentTypeID];

			auto& componentInterface = storage->m_Components[componentID];

			FL_CORE_ASSERT(componentInterface, "An attempt to remove a component that the entity doesn't have");

			GetComponentPool<T>()->Release(componentInterface);

			storage->Unstore(entityID, componentID, componentTypeID);
		}

		void RemoveAllComponents(ComponentStorage* storage, const IEntity* entityInterface);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD T* GetComponent(ComponentStorage* storage, const IEntity* entityInterface) const
		{
			const auto componentTypeID = T::s_TypeID;

			const auto entityID = entityInterface->m_UniqueID;

			const auto componentID = storage->m_ComponentMap[entityID][componentTypeID];

			if (componentID == InvalidComponentTypeID) return nullptr;

			return FastCast<T*>(storage->m_Components[componentID]);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD bool HasComponent(ComponentStorage* storage, const IEntity* entityInterface) const
		{
			const auto componentTypeID = T::s_TypeID;

			const auto entityID = entityInterface->m_UniqueID;

			const auto componentID = storage->m_ComponentMap[entityID][componentTypeID];

			if (componentID == InvalidComponentTypeID) return false;

			return true;
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD ComponentPool<T>* GetComponentPool()
		{
			ComponentPool<T>* pool = nullptr;

			const auto componentTypeID = T::s_TypeID;

			const auto& it = m_ComponentPools.find(componentTypeID);

			if (it == m_ComponentPools.end() || !it->second)
			{
				pool = new ComponentPool<T>;

				FL_CORE_TRACE("Creating component pool of type [{0}]...", pool->GetTypeName());

				m_ComponentPools[componentTypeID] = pool;
			}
			else
			{
				pool = FastCast<ComponentPool<T>*>(it->second);
			}

			FL_CORE_ASSERT(pool, "Failed to create component pool");

			return pool;
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD inline ComponentIterator<T> Begin()
		{
			return GetComponentPool<T>()->Begin();
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD inline ComponentIterator<T> End()
		{
			return GetComponentPool<T>()->End();
		}

		void Init() override;
		void Dispose() override;
	};
}
