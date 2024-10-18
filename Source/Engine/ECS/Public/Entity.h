#pragma once

#include "Platform.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class FEntity final
	{
		friend class FEntityManager;
		friend class FComponentManager;

	public:
		FEntity(InstanceID_t InstanceID);

		void operator delete(void* Block) = delete;
		void operator delete[](void* Block) = delete;

		bool bActive = true;

		NODISCARD FORCEINLINE InstanceID_t GetInstanceID() const noexcept
		{
			return InstanceID;
		}

		void Destroy();
		void RemoveAllComponents();

	private:
		InstanceID_t InstanceID = 0;
	};
}
