#pragma once

#include "Memory/FixedAllocator.h"
#include "Utility/Utility.h"
#include "EntityStorage.h"

namespace Bloodshot
{
	class EntityManager final : public ISingleton<EntityManager>
	{
		OWNED_BY_CORE;
		ECS_PART;

	public:
		template<typename T, typename... Args>
			requires std::is_base_of_v<IEntity, T>
		NODISCARD std::pair<IEntity*, T*> Instantiate(EntityStorage* storage, Args&&... args)
		{
			auto memory = GetEntityPool<T>()->Allocate();

			IEntity* entityInterface = new(memory) T(std::forward<Args>(args)...);

			storage->Store(entityInterface);

			return std::make_pair(entityInterface, FastCast<T*>(entityInterface));
		}

		void Destroy(EntityStorage* storage, IEntity* entityInterface);

	private:
		class IEntityPool abstract
		{
		public:
			ECS_PART;

			virtual ~IEntityPool() {}

			NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;
			virtual void Release(IEntity* ptr) = 0;
		};

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		class EntityPool final : public IEntityPool, public FixedAllocator<T>
		{
		public:
			EntityPool()
				: FixedAllocator<T>()
			{
			}

			EntityPool(const EntityPool& other) = delete;

			~EntityPool() override
			{
				FixedAllocator<T>::Release();
			}

			EntityPool& operator=(const EntityPool& other) = delete;

			NODISCARD FORCEINLINE const char* GetTypeName() const noexcept override
			{
				static const char* typeName{typeid(T).name()};
				return typeName;
			}

			void Release(IEntity* entityInterface) override
			{
				FixedAllocator<T>::Deallocate(entityInterface);
			}

		private:
			friend class IEntity;
		};

		std::unordered_map<EntityTypeID, IEntityPool*> m_EntityPools;

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		EntityPool<T>* GetEntityPool()
		{
			EntityPool<T>* pool = nullptr;

			const auto entityTypeID = T::s_TypeID;

			const auto& it = m_EntityPools.find(entityTypeID);

			if (it == m_EntityPools.end() || it->second == nullptr)
			{
				pool = new EntityPool<T>;

				FL_CORE_TRACE("Creating entity pool of type [{0}]...", pool->GetTypeName());

				m_EntityPools[entityTypeID] = pool;
			}
			else
			{
				pool = FastCast<EntityPool<T>*>(it->second);
			}

			FL_CORE_ASSERT(pool, "Failed to create entity pool");

			return pool;
		}

		void Init() override;
		void Dispose() override;

		friend class ECS;
	};
}
