#pragma once

#include "Core.h"

#include "Entity.h"

namespace Bloodshot
{
	class FEntityManager final : public TManager<FEntityManager>
	{
		friend class IEngineContext;
		friend class FScene;
		friend class FComponentManager;

	public:
		using FEntityAllocator = TBlockAllocator<FEntity>;
		using FEntityArray = TArray<TReference<FEntity>>;

		static inline size_t EntityStorageGrow = 1024;

		static TReference<FEntity> Instantiate();
		static void InstantiateMultiple(const size_t Count);
		static void InstantiateMultiple(FEntityArray& OutResult, const size_t Count);

		template<size_t Count>
		static void InstantiateMultiple(TStaticArray<TReference<FEntity>, Count>& OutResult)
		{
			BS_PROFILE_FUNCTION();

			for (TReference<FEntity>& Entity : OutResult)
			{
				Entity = Instantiate();
			}
		}

		static void Destroy(TReference<FEntity> Entity);
		static void DestroyMultiple(FEntityArray& OutEntities);

	private:
		FEntityManager();

		// BSTODO: Temp
		FEntityAllocator EntityAllocator{1024, 128};
		FEntityArray Entities;
		TList<InstanceID_t> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

		static void DestroyAllEntities();

		NODISCARD static InstanceID_t Reserve();

		static void Store(const InstanceID_t InstanceID, TReference<FEntity> Entity);

		static void Unstore(const InstanceID_t EntityInstanceID);

		NODISCARD static bool Contains(const InstanceID_t EntityInstanceID);

		static void Resize(const size_t NewSize);
	};
}
