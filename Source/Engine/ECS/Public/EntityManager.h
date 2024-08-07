#pragma once

#include "BlockAllocator.h"
#include "EntityStorage.h"
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
		friend struct IECS;

	public:
		using IEntityAllocator = IAllocator;
		template<typename T>
		using TEntityAllocator = TBlockAllocator<T>;
		using FTypeIDEntityAllocatorUnorderedMap = std::unordered_map<TypeID_t, IEntityAllocator*>;

		FEntityManager();

		FTypeIDEntityAllocatorUnorderedMap EntityAllocatorsMap;

		virtual void Init() override;
		virtual void Dispose() override;

	private:
		template<typename T>
			requires std::is_base_of_v<IEntity, T>
		NODISCARD static TEntityAllocator<T>* GetOrCreateEntityAllocator()
		{
			using FEntityAllocator = TEntityAllocator<T>;

			FTypeIDEntityAllocatorUnorderedMap& EntityAllocatorsMap = Instance->EntityAllocatorsMap;

			const TypeID_t EntityTypeID = T::TypeID;

			FTypeIDEntityAllocatorUnorderedMap::const_iterator It = EntityAllocatorsMap.find(EntityTypeID);

			if (It != EntityAllocatorsMap.end() && It->second)
			{
				return ReinterpretCast<FEntityAllocator*>(It->second);
			}

			FEntityAllocator* const Allocator = new FEntityAllocator({1024, 100});

			BS_LOG(Trace, "Creating EntityPool of type: {0}...", TTypeInfo<T>::GetTypeName());

			EntityAllocatorsMap[EntityTypeID] = Allocator;

			return Allocator;
		}

		NODISCARD static IEntityAllocator* GetEntityAllocator(const TypeID_t EntityTypeID);

		template<typename T, typename... ArgTypes>
			requires std::is_base_of_v<IEntity, T>
		/*NODISCARD*/ static T* Instantiate(TUniquePtr<FEntityStorage>& Storage, ArgTypes&&... Args)
		{
			IEntity* const Entity = new(GetOrCreateEntityAllocator<T>()->Allocate()) T(std::forward<ArgTypes>(Args)...);

			Storage->Store(Entity);

			Entity->BeginPlay();

			return ReinterpretCast<T*>(Entity);
		}

		static void Destroy(TUniquePtr<FEntityStorage>& Storage, IEntity* const Entity);
	};
}
