#pragma once

#include "Core/EngineFramework.h"
#include "Utility/TypeInfo.h"
#include "Utility/Utility.h"

namespace Bloodshot
{
	using SystemID = InstanceID;
	using SystemTypeID = TypeID;

	class ISystem abstract
	{
		ECS_PART;

	public:
		ISystem() {}
		~ISystem() {}

		NODISCARD FORCEINLINE virtual SystemTypeID GetTypeID() const noexcept = 0;

		NODISCARD FORCEINLINE virtual const char* GetTypeName() const noexcept = 0;

		NODISCARD FORCEINLINE SystemID GetUniqueID() const noexcept
		{
			return m_UniqueID;
		}

	protected:
		SystemID m_UniqueID = 0;

		bool m_Enabled = true;

		virtual void FixedTick() {}
		virtual void Tick(float deltaTime) {}
		virtual void LateTick(float deltaTime) {}

		friend class Scene;
	};
}
