#pragma once

#include "ISystem.h"

namespace Bloodshot
{
	template<typename T>
	class System abstract : public ISystem
	{
		ECS_MODULE;

	public:
		System() {}
		~System() {}

		static const SystemTypeID s_TypeID;

		FORCEINLINE SystemTypeID GetTypeID() const noexcept override
		{
			return s_TypeID;
		}

		FORCEINLINE const char* GetTypeName() const noexcept override
		{
			static const char* typeName{typeid(T).name()};
			return typeName;
		}

	private:
		using ISystem::m_UniqueID;
	};

	template<typename T>
	const SystemTypeID System<T>::s_TypeID = TypeInfo<ISystem>::GetTypeID<T>();
}
