#pragma once

#include "Utility/Utility.h"
#include "Utility/TypeInfo.h"
#include "Core/Assert.h"

namespace Bloodshot
{
	template<typename T>
	class ISingleton
	{
	public:
		ISingleton(const ISingleton& other) = delete;

		ISingleton& operator=(const ISingleton& other) = delete;

		static T* Create()
		{
			FL_CORE_ASSERT(!s_Instance, "An attempt to create another singleton instance of {0}", TypeInfo<T>::GetTypeName());

			if (s_Instance) return s_Instance;

			s_Instance = new T;

			return s_Instance;
		}

	protected:
		ISingleton() {}

		virtual void Init() = 0;
		virtual void Dispose() = 0;

		static T* s_Instance;
	};

	template<typename T>
	T* ISingleton<T>::s_Instance = nullptr;
}
