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
		friend class IECS;

	public:
		using FEntityAllocator = TBlockAllocator<FEntity>;

		FEntityManager();

		FEntityAllocator EntityAllocator;
		std::vector<TReference<FEntity>> EntityVec;
		std::list<InstanceID_t> FreeSlotsList;

		virtual void Init() override;
		virtual void Dispose() override;

	private:
		static TReference<FEntity> Instantiate();

		static void Destroy(TReference<FEntity> Entity);

		NODISCARD static InstanceID_t Reserve();

		static void Store(const InstanceID_t InstanceID, TReference<FEntity> Entity);

		static void Unstore(const InstanceID_t EntityInstanceID);

		NODISCARD static bool Contains(const InstanceID_t EntityInstanceID);

		static void Resize(const size_t NewSize);
	};
}
