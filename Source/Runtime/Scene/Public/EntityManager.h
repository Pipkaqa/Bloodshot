#pragma once

#include "Core.h"

#include "Entity.h"

namespace Bloodshot
{
	class FEntityManager final
	{
		friend class IEngineContext;
		friend class FScene;
		friend class FComponentManager;

	public:
		using FEntityArray = TArray<TReference<FEntity>>;

		static FEntityManager& GetInstance();

		static TReference<FEntity> Instantiate();
		static FEntityArray Instantiate(const size_t Count);
		static void Instantiate(FEntityArray& OutResult, const size_t Count);

		template<size_t Count>
		static void Instantiate(TStaticArray<TReference<FEntity>, Count>& OutResult)
		{
			BS_PROFILE_FUNCTION();

			for (TReference<FEntity>& Entity : OutResult)
			{
				Entity = Instantiate();
			}
		}

		static void Destroy(TReference<FEntity> Entity);
		static void Destroy(FEntityArray& OutEntities);

		template<size_t Count>
		static void Destroy(TStaticArray<TReference<FEntity>, Count>& OutEntities)
		{
			BS_PROFILE_FUNCTION();

			for (TReference<FEntity> Entity : OutEntities)
			{
				Destroy(Entity);
			}
		}

	private:
		FEntityManager() {}

		TUnorderedMap<size_t, TReference<FEntity>> Entities;

		void DestroyAllEntities();

		NODISCARD static bool Contains(const size_t EntityUniqueID);
	};
}
