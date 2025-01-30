#pragma once

#include "Core.h"

namespace Bloodshot
{
	struct FSystemInfo final
	{
		size_t Size = 0;
		const char* TypeName = "Unknown";
	};

	BSCLASS();
	class ISystem : public FObject
	{
		friend class FSystemManager;
		friend class FScene;

	public:
		virtual ~ISystem() {}

		BSPROPERTY(Serialized, Replicated);
		bool bEnabled = true;

		NODISCARD FORCEINLINE InstanceID_t GetInstanceID() const noexcept
		{
			return InstanceID;
		}

		NODISCARD FORCEINLINE TypeID_t GetTypeID() const noexcept
		{
			return TypeID;
		}

		NODISCARD FORCEINLINE FSystemInfo GetInfo() const noexcept
		{
			return Info;
		}

	private:
		InstanceID_t InstanceID = 0;
		TypeID_t TypeID = 0;
		FSystemInfo Info = {};

		virtual void FixedTick() {}
		virtual void Tick(float DeltaTime) {}
		virtual void LateTick(float DeltaTime) {}
	};
}
