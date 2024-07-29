#pragma once

#include "Utility.h"

namespace Bloodshot
{
	using InstanceID = uint64_t;
	using TypeID = uint32_t;

	template<typename T>
	class TypeInfo
	{
	public:
		static const InstanceID s_InvalidInstanceID = std::numeric_limits<InstanceID>::max();
		static const TypeID s_InvalidTypeID = std::numeric_limits<TypeID>::max();

		NODISCARD FORCEINLINE static TypeID GetTypeID()
		{
			static const TypeID typeID{s_ObjectsCount++};
			return typeID;
		}

		template<typename U>
		NODISCARD FORCEINLINE static TypeID GetTypeID()
		{
			static const TypeID typeID{s_ObjectsCount++};
			return typeID;
		}

		NODISCARD FORCEINLINE static const char* GetTypeName()
		{
			static const char* typeName{typeid(T).name()};
			return typeName;
		}

		NODISCARD FORCEINLINE static TypeID GetObjectsCount()
		{
			return s_ObjectsCount;
		}

	private:
		static TypeID s_ObjectsCount;
	};

	template<typename T>
	TypeID TypeInfo<T>::s_ObjectsCount = 0;
}