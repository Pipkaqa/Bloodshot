#pragma once

#include "Core.h"

namespace Bloodshot
{
	BSCLASS();
	class ISystem : public IObject
	{
		friend class FSystemManager;
		friend class FScene;

		GENERATED_BODY();

	public:
		virtual ~ISystem() = default;

		BSPROPERTY(Serialized, Replicated);
		bool bEnabled = true;

	private:
		virtual void FixedTick() {}
		virtual void Tick(float DeltaTime) {}
		virtual void LateTick(float DeltaTime) {}
	};
}
