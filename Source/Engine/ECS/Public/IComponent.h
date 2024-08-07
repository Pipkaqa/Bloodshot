#pragma once

#include "Platform.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class IEntity;

	class IComponent abstract
	{
		friend struct FComponentStorage;
		friend class FComponentManager;

	public:
		bool bActive = true;

		NODISCARD FORCEINLINE virtual TypeID_t GetTypeID() const noexcept = 0;

		NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;

		NODISCARD FORCEINLINE InstanceID_t GetUniqueID() const noexcept
		{
			return UniqueID;
		}

		NODISCARD FORCEINLINE IEntity* GetOwner() const noexcept
		{
			return Owner;
		}

	protected:
		InstanceID_t UniqueID = 0;
		IEntity* Owner = nullptr;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};
}
