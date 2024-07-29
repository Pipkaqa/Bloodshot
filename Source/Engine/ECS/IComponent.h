#pragma once

#include "Core/EngineFramework.h"
#include "Utility/TypeInfo.h"

namespace Bloodshot
{
	using ComponentID = InstanceID;
	using ComponentTypeID = TypeID;

	class IEntity;

	class IComponent abstract
	{
		ECS_MODULE;

	public:
		IComponent() {}
		~IComponent() {}

		bool m_Enabled = true;

		NODISCARD FORCEINLINE virtual ComponentTypeID GetTypeID() const noexcept = 0;

		NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;

		NODISCARD FORCEINLINE ComponentID GetUniqueID() const noexcept
		{
			return m_UniqueID;
		}

		NODISCARD FORCEINLINE IEntity* GetOwner() const noexcept
		{
			return m_Owner;
		}

	protected:
		ComponentID m_UniqueID = 0;
		IEntity* m_Owner = nullptr;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}

		friend class ComponentStorage;
	};
}
