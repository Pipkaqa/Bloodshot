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
		friend class FComponentManager;
		friend struct IECS;

	public:
		using IEntityAllocator = IAllocator;
		using FEntityAllocator = TBlockAllocator<FEntity>;
		using FTypeIDEntityAllocatorUnorderedMap = std::unordered_map<TypeID_t, FEntityAllocator>;

		FEntityManager();

		FEntityAllocator EntityAllocator;
		std::vector<FEntity*> EntityVec;
		std::list<InstanceID_t> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

	private:
		static FEntity* Instantiate();

		static void Destroy(FEntity* const Entity);

		NODISCARD static InstanceID_t Reserve();

		static void Store(const InstanceID_t InstanceID, FEntity* const Entity);

		static void Unstore(const InstanceID_t EntityInstanceID);

		NODISCARD static bool Contains(const InstanceID_t EntityInstanceID);

		static void Resize(const size_t NewSize);
	};
}
