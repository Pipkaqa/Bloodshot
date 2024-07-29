#pragma once

#include "Core/Assert.h"
#include "INonCopyable.h"
#include "Utility/TypeInfo.h"

namespace Bloodshot
{
	template<typename T>
	class ISingleton : public INonCopyable
	{
	public:
		static T* Create()
		{
			BS_ASSERT(!s_Instance, "An attempt to create another singleton instance of type {0}", TypeInfo<T>::GetTypeName());

			s_Instance = new T;

			return s_Instance;
		}

	protected:
		ISingleton() {}

		static T* s_Instance;

		virtual void Init() = 0;
		virtual void Dispose() = 0;
	};

	template<typename T>
	T* ISingleton<T>::s_Instance = nullptr;

	template<typename T>
	class IStaticSingleton : public INonCopyable
	{
	public:
		static T* Create()
		{
			BS_ASSERT(!s_Instance, "An attempt to create another singleton instance of type {0}", TypeInfo<T>::GetTypeName());

			s_Instance = new T;

			return s_Instance;
		}

	protected:
		IStaticSingleton() {}

		static T* s_Instance;
	};

	template<typename T>
	T* IStaticSingleton<T>::s_Instance = nullptr;
}
