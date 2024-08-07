#include "EntityManager.h"

#include "Entity.h"
#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	FEntityManager::FEntityManager()
	{
		Instance = this;
	}

	void FEntityManager::Init()
	{
		BS_LOG(Debug, "Creating EntityManager...");
	}

	void FEntityManager::Dispose()
	{
		BS_LOG(Debug, "Destroying EntityManager...");

		FTypeIDEntityAllocatorUnorderedMap& EntityAllocatorsMap = Instance->EntityAllocatorsMap;

		for (const FTypeIDEntityAllocatorUnorderedMap::value_type& PoolPair : EntityAllocatorsMap)
		{
			IEntityAllocator* const Allocator = PoolPair.second;

			if (!Allocator) continue;

			BS_LOG(Trace, "Destroying EntityPool of undefined type...");

			delete Allocator;
		}

		EntityAllocatorsMap.clear();
	}

	FEntityManager::IEntityAllocator* FEntityManager::GetEntityAllocator(const TypeID_t EntityTypeID)
	{
		FTypeIDEntityAllocatorUnorderedMap& EntityAllocatorsMap = Instance->EntityAllocatorsMap;

		FTypeIDEntityAllocatorUnorderedMap::const_iterator It = EntityAllocatorsMap.find(EntityTypeID);

		IEntityAllocator* const Allocator = It->second;

		BS_ASSERT(It != EntityAllocatorsMap.end() && Allocator, "Attempting to get not existing EntityPool");

		return Allocator;
	}

	void FEntityManager::Destroy(TUniquePtr<FEntityStorage>& Storage, IEntity* Entity)
	{
		InstanceID_t EntityInstanceID = Entity->UniqueID;

		// BSNOTE: Assert -> Error (06.08.2024)
		//BS_ASSERT(EntityInstanceID != InvalidInstanceID && Storage->Contains(EntityInstanceID), 
		//	"Attempting to destroy not existing Entity");
		if (EntityInstanceID == InvalidInstanceID || !Storage->Contains(EntityInstanceID))
		{
			BS_LOG(Error, "Attempting to destroy not existing Entity");
			return;
		}

		BS_LOG(Trace, "Destroying Entity of type: {0}...", Entity->GetTypeName());

		Entity->EndPlay();

		GetEntityAllocator(Entity->GetTypeID())->Deallocate(Entity);

		Storage->Unstore(EntityInstanceID);
	}
}
