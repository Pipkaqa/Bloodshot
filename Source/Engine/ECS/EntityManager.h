#pragma once

#include "EntityStorage.h"
#include "Memory/FixedAllocator.h"

namespace Bloodshot
{
	class EntityManager final : public ISingleton<EntityManager>
	{
		CORE_MODULE;
		ECS_MODULE;

		using ISingleton::Create;

		class IEntityPool abstract
		{
		public:
			ECS_MODULE;

			virtual ~IEntityPool() {}

			NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;
			virtual void Release(IEntity* entityInterface) = 0;
		};

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		class EntityPool final : public IEntityPool, public FixedAllocator<T>, public INonCopyable
		{
			ECS_MODULE;

			EntityPool() {}
			~EntityPool() override {}

			NODISCARD FORCEINLINE const char* GetTypeName() const noexcept override
			{
				static const char* typeName{typeid(T).name()};
				return typeName;
			}

			void Release(IEntity* entityInterface) override
			{
				this->Deallocate(entityInterface);
			}
		};

		std::unordered_map<EntityTypeID, IEntityPool*> m_EntityPools;

		void Init() override;
		void Dispose() override;

		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		NODISCARD EntityPool<T>* GetEntityPool()
		{
			EntityPool<T>* pool = nullptr;

			const auto entityTypeID = T::s_TypeID;

			const auto& it = m_EntityPools.find(entityTypeID);

			if (it == m_EntityPools.end() || it->second == nullptr)
			{
				pool = new EntityPool<T>;

				BS_TRACE("Creating entity pool of type [{0}]...", pool->GetTypeName());

				m_EntityPools[entityTypeID] = pool;
			}
			else
			{
				pool = FastCast<EntityPool<T>*>(it->second);
			}

			BS_ASSERT(pool, "Failed to create entity pool");

			return pool;
		}

		template<typename T, typename... Args>
			requires std::is_base_of_v<IEntity, T>
		NODISCARD T* Instantiate(EntityStorage* storage, Args&&... args)
		{
			auto memory = GetEntityPool<T>()->Allocate();

			IEntity* entityInterface = new(memory) T(std::forward<Args>(args)...);

			storage->Store(entityInterface);

			entityInterface->BeginPlay();

			return FastCast<T*>(entityInterface);
		}

		void Destroy(EntityStorage* storage, IEntity* entityInterface);
	};
}
