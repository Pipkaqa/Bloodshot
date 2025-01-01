#pragma once

#include "BlockAllocator.h"
#include "Entity.h"
#include "Logging/LoggingMacros.h"
#include "Platform.h"
#include "Templates/Singleton.h"
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"

#include <unordered_map>

namespace Bloodshot
{
	class FEntityManager final : public TSingleton<FEntityManager>
	{
		friend class FScene;
		friend class FComponentManager;

	public:
		using FEntityAllocator = TBlockAllocator<FEntity>;

		FEntityManager();

		static inline size_t EntityStorageGrow = 1024;

		FEntityAllocator EntityAllocator;
		std::vector<TReference<FEntity>> EntityVec;
		std::list<InstanceID_t> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

		static TReference<FEntity> Instantiate();
		static void InstantiateMultiple(const size_t Count);
		static void InstantiateMultiple(std::vector<TReference<FEntity>>& OutResult, const size_t Count);

		template<size_t Count>
		static void InstantiateMultiple(std::array<TReference<FEntity>, Count>& OutResult)
		{
			BS_PROFILE_FUNCTION();

			for (TReference<FEntity>& Entity : OutResult)
			{
				Entity = Instantiate();
			}
		}

		static void Destroy(TReference<FEntity> Entity);
		static void DestroyMultiple(std::vector<TReference<FEntity>>& OutEntities);

	private:
		static void DestroyAllEntities();

		NODISCARD static InstanceID_t Reserve();

		static void Store(const InstanceID_t InstanceID, TReference<FEntity> Entity);

		static void Unstore(const InstanceID_t EntityInstanceID);

		NODISCARD static bool Contains(const InstanceID_t EntityInstanceID);

		static void Resize(const size_t NewSize);
	};
}
