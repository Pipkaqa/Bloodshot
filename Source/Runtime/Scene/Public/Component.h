#pragma once

#include "Core.h"

namespace Bloodshot
{
	class FEntity;

	BSCLASS();
	class IComponent : public IObject
	{
		friend class FEntity;

		GENERATED_BODY();

	public:
		BSPROPERTY(Serialized, Replicated);
		bool bActive = true;

		NODISCARD FORCEINLINE FEntity* GetOwner() noexcept
		{
			return Owner;
		}

	protected:
		FEntity* Owner;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};

	template<typename T>
	concept IsComponent = std::is_base_of_v<IComponent, T>;
}
