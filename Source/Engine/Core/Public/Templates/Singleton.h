#pragma once

#include "AssertionMacros.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class INonCopyable
	{
	public:
		INonCopyable() {}
		INonCopyable(const INonCopyable& Other) = delete;

		INonCopyable& operator=(const INonCopyable& Other) = delete;
	};

	template<typename InElementType>
	class TSingleton : public INonCopyable
	{
	public:
		TSingleton()
		{
			BS_ASSERT(!Instance, "Attempting to create second Singleton of type: {0}", TTypeInfo<InElementType>::GetTypeName());
		}

		virtual void Init() = 0;
		virtual void Dispose() = 0;

	protected:
		static inline InElementType* Instance = nullptr;
	};

	template<typename InElementType>
	class TStaticSingleton : public INonCopyable
	{
	public:
		TStaticSingleton()
		{
			BS_ASSERT(!Instance, "Attempting to create second Singleton of type: {0}", TTypeInfo<InElementType>::GetTypeName());
		}

	protected:
		static inline InElementType* Instance = nullptr;
	};
}
