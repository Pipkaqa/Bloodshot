#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FTransformComponent;

	class FEntity final
	{
		friend class FEntityManager;
		friend class FComponentManager;

	public:
		FEntity(const InstanceID_t InstanceID);

		void operator delete(void* Block) = delete;
		void operator delete[](void* Block) = delete;

		bool bActive = true;

		NODISCARD FORCEINLINE InstanceID_t GetInstanceID() const noexcept
		{
			return InstanceID;
		}

		NODISCARD FORCEINLINE TReference<FTransformComponent> GetTransformComponent() const noexcept
		{
			return TransformComponent;
		}

		void Destroy();
		void RemoveAllComponents();

	private:
		InstanceID_t InstanceID = 0;
		TReference<FTransformComponent> TransformComponent = nullptr;
	};
}
