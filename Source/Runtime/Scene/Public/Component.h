#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FEntity;

	BSCLASS();
	class IComponent : public IObject
	{
		friend class FComponentManager;

		GENERATED_BODY();

	public:
		BSPROPERTY(Serialized, Replicated);
		bool bActive = true;

		NODISCARD FORCEINLINE TReference<FEntity> GetOwner() noexcept
		{
			return Owner;
		}

	protected:
		TReference<FEntity> Owner;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};
}
