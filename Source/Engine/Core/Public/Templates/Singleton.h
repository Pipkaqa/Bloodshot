#pragma once

#include "AssertionMacros.h"
#include "TypeInfo.h"

namespace Bloodshot
{
	class INonCopyable abstract
	{
	public:
		INonCopyable() {}
		INonCopyable(const INonCopyable& Other) = delete;

		INonCopyable& operator=(const INonCopyable& Other) = delete;
	};

	template<typename T>
	class TSingleton : public INonCopyable
	{
	public:
		TSingleton()
		{
			BS_ASSERT(!Instance, "Attempting to create second Singleton of type: {0}", TTypeInfo<T>::GetTypeName());
		}

		static inline T* Instance = nullptr;

		virtual void Init() = 0;
		virtual void Dispose() = 0;
	};

	template<typename T>
	class TStaticSingleton : public INonCopyable
	{
	public:
		TStaticSingleton()
		{
			BS_ASSERT(!Instance, "Attempting to create second Singleton of type: {0}", TTypeInfo<T>::GetTypeName());
		}

		static inline T* Instance = nullptr;
	};
}
