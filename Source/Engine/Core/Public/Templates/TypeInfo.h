#pragma once

#include "Platform.h"

#include <limits>
#include <typeinfo>

namespace Bloodshot
{
	// BSTODO: #AFTER_REFLECTION, Rewrite whole TTypeInfo class
	using InstanceID_t = uint64_t;
	using TypeID_t = uint32_t;

	static constexpr InstanceID_t InvalidInstanceID = std::numeric_limits<InstanceID_t>::max();
	static constexpr TypeID_t InvalidTypeID = std::numeric_limits<TypeID_t>::max();

	template<typename T>
	class TTypeInfo abstract
	{
	public:
		template<typename U>
		NODISCARD FORCEINLINE static TypeID_t GetTypeID() noexcept
		{
			static const TypeID_t TypeID{TypeIDCounter++};
			return TypeID;
		}

		NODISCARD FORCEINLINE static TypeID_t GetTypeID() noexcept
		{
			static const TypeID_t TypeID{TypeIDCounter++};
			return TypeID;
		}

		NODISCARD FORCEINLINE static const char* GetTypeName() noexcept
		{
			static const char* TypeName{typeid(T).name()};
			return TypeName;
		}

	private:
		static inline TypeID_t TypeIDCounter = 0;
	};
}
