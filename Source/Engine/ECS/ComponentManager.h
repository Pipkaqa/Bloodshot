#pragma once

#include "ComponentStorage.h"
#include "Debug/Logger.h"
#include "Memory/FixedAllocator.h"

namespace Bloodshot
{
	class ComponentManager final : public ISingleton<ComponentManager>
	{
		CORE_MODULE;
		ECS_MODULE;

		using ISingleton::Create;

		class IComponentPool abstract
		{
			ECS_MODULE;

			virtual ~IComponentPool() {}

			NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;
			virtual void Release(IComponent* componentInterface) = 0;
		};

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		class ComponentPool final : public IComponentPool, public FixedAllocator<T>, public INonCopyable
		{
			ECS_MODULE;

			ComponentPool() {}
			~ComponentPool() override {}

			NODISCARD FORCEINLINE const char* GetTypeName() const noexcept override
			{
				static const char* typeName{typeid(T).name()};
				return typeName;
			}

			void Release(IComponent* componentInterface) override
			{
				this->Deallocate(componentInterface);
			}
		};

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		using ComponentIterator = ComponentPool<T>::Iterator;

		std::unordered_map<ComponentTypeID, IComponentPool*> m_ComponentPools;

		void Init() override;
		void Dispose() override;

		template<typename T, typename... Args>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD T* AddComponent(ComponentStorage* storage, IEntity* entityInterface, Args&&... args)
		{
			const auto& componentTypeID = T::s_TypeID;

			const auto memory = GetComponentPool<T>()->Allocate();

			IComponent* componentInterface = new(memory) T(std::forward<Args>(args)...);

			storage->Store(entityInterface, componentInterface, componentTypeID);

			componentInterface->BeginPlay();

			return FastCast<T*>(componentInterface);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		void RemoveComponent(ComponentStorage* storage, IEntity* entityInterface)
		{
			const auto componentTypeID = T::s_TypeID;

			const auto entityID = entityInterface->m_UniqueID;

			const auto componentID = storage->m_ComponentMap[entityID][componentTypeID];

			//if (componentID == InvalidComponentID) return;

			//BS_ASSERT((componentID != InvalidComponentTypeID && componentID < components.size()),
			//	"An attempt to destroy not existing component");

			auto& componentInterface = storage->m_Components[componentID];

			// BSTODO: #AFTER_EDITOR, Write error in editor console

			BS_ASSERT(componentInterface, "An attempt to remove a component that the entity doesn't have");

			componentInterface->EndPlay();

			GetComponentPool<T>()->Release(componentInterface);

			storage->Unstore(entityID, componentID, componentTypeID);
		}

		void RemoveAllComponents(ComponentStorage* storage, IEntity* entityInterface);

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD T* GetComponent(const ComponentStorage* storage, const IEntity* entityInterface) const
		{
			const auto componentTypeID = T::s_TypeID;

			const auto entityID = entityInterface->m_UniqueID;

			const auto componentID = storage->m_ComponentMap.find(entityID)->second.find(componentTypeID)->second;

			BS_ASSERT(componentID != InvalidComponentID, "An attempt to get not existing component");

			return FastCast<T*>(storage->m_Components[componentID]);
		}

		template<typename T>
			requires std::is_base_of_v<IComponent, T>
		NODISCARD bool HasComponent(const ComponentStorage* storage, const IEntity* entityInterface) const
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

				BS_TRACE("Creating component pool of type [{0}]...", pool->GetTypeName());

				m_ComponentPools[componentTypeID] = pool;
			}
			else
			{
				pool = FastCast<ComponentPool<T>*>(it->second);
			}

			BS_ASSERT(pool, "Failed to create component pool");

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
	};
}
