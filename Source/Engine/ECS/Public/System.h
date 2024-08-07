#pragma once

#include "ISystem.h"

namespace Bloodshot
{
	template<typename T>
	class TSystem abstract : public ISystem
	{
	public:
		static inline const TypeID_t TypeID = TTypeInfo<ISystem>::GetTypeID();

		virtual TypeID_t GetTypeID() const noexcept override
		{
			return TypeID;
		}

		virtual const char* GetTypeName() const noexcept override
		{
			static const char* TypeName{typeid(T).name()};
			return TypeName;
		}

	private:
		using ISystem::UniqueID;
	};
}
