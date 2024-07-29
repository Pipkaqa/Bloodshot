#pragma once

#include "Core/EngineFramework.h"
#include "Utility/TypeInfo.h"

namespace Bloodshot
{
	using EntityID = InstanceID;
	using EntityTypeID = TypeID;

	class IEntity abstract
	{
		ECS_MODULE;

	public:
		IEntity() {}
		~IEntity() {}

		bool m_Enabled = true;

		NODISCARD FORCEINLINE virtual EntityTypeID GetTypeID() const noexcept = 0;

		NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;

		NODISCARD FORCEINLINE EntityID GetUniqueID() const noexcept
		{
			return m_UniqueID;
		}

	protected:
		EntityID m_UniqueID = 0;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};
}
