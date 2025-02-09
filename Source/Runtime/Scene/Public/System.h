#pragma once

#include "Core.h"

#include "Handle.h"

namespace Bloodshot
{
	BSCLASS();
	class ISystem : public IObject
	{
		friend class FSystemManager;
		friend class FScene;

		GENERATED_BODY();

	public:
		BSPROPERTY(Serialized, Replicated);
		bool bEnabled = true;

	private:
		FInstanceID InstanceID;
		FTypeID TypeID;

		virtual void FixedTick() {}
		virtual void Tick(float DeltaTime) {}
		virtual void LateTick(float DeltaTime) {}
	};
}
