#pragma once

#include "Platform.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class IEntity abstract
	{
		friend struct FEntityStorage;
		friend class FEntityManager;

	public:
		bool bActive = true;

		NODISCARD FORCEINLINE virtual TypeID_t GetTypeID() const noexcept = 0;

		NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;

		NODISCARD FORCEINLINE InstanceID_t GetUniqueID() const noexcept
		{
			return UniqueID;
		}

	protected:
		InstanceID_t UniqueID = 0;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};
}
