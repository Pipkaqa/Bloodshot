#pragma once

#include "Core.h"

#include "Entity.h"
#include "Handle.h"

namespace Bloodshot
{
	class FEntityManager final : public TManager<FEntityManager>
	{
		friend class IEngineContext;
		friend class FScene;
		friend class FComponentManager;

	public:
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

		FEntityArray Entities;
		TList<FInstanceID> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

		static void DestroyAllEntities();

		NODISCARD static FInstanceID Reserve();

		static void Store(const FInstanceID InstanceID, TReference<FEntity> Entity);

		static void Unstore(const FInstanceID EntityInstanceID);

		NODISCARD static bool Contains(const FInstanceID EntityInstanceID);

		static void Resize(const size_t NewSize);
	};
}
