#pragma once

#include "Core.h"

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
		BSPROPERTY(Serialized);
		FString Name = "Entity";

		BSPROPERTY(Serialized, Replicated);
		bool bActive = true;

		NODISCARD FORCEINLINE TReference<FTransformComponent> GetTransformComponent() noexcept
		{
			return TransformComponent;
		}

		void Destroy();
		void RemoveAllComponents();

	private:
		TReference<FTransformComponent> TransformComponent;
	};
}
