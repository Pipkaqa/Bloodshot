#pragma once

#include "Platform.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class ISystem abstract
	{
		friend struct FSystemStorage;
		friend class IScene;

	public:
		bool bEnabled = true;

		NODISCARD FORCEINLINE virtual TypeID_t GetTypeID() const noexcept = 0;

		NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;

		NODISCARD FORCEINLINE InstanceID_t GetUniqueID() const noexcept
		{
			return UniqueID;
		}

	protected:
		InstanceID_t UniqueID = 0;

		virtual void FixedTick() {}
		virtual void Tick(float DeltaTime) {}
		virtual void LateTick(float DeltaTime) {}
	};
}
