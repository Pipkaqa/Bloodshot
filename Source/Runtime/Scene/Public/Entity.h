#pragma once

#include "Core.h"

#include "Handle.h"

namespace Bloodshot
{
	class FTransformComponent;

	BSCLASS();
	class FEntity final : public IObject
	{
		friend class FEntityManager;
		friend class FComponentManager;

		GENERATED_BODY();

	public:
		FEntity(const FInstanceID InstanceID);

		BSPROPERTY(Serialized, Replicated);
		bool bActive = true;

		NODISCARD FORCEINLINE TReference<FTransformComponent> GetTransformComponent() noexcept
		{
			return TransformComponent;
		}

		void Destroy();
		void RemoveAllComponents();

	private:
		FInstanceID InstanceID;
		TReference<FTransformComponent> TransformComponent;
	};
}
