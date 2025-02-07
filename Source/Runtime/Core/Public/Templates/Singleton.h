#pragma once

#include "Templates/SmartPointers.h"

namespace Bloodshot
{
	class INonCopyable
	{
	public:
		INonCopyable() = default;
		INonCopyable(const INonCopyable& Other) = delete;
		INonCopyable& operator=(const INonCopyable& Other) = delete;
	};

	template<typename InElementType>
	class TSingleton : public INonCopyable
	{
	public:
		TSingleton() = default;

		static TReference<InElementType> GetInstance()
		{
			return Instance;
		}

	protected:
		static inline InElementType* Instance = nullptr;
	};

	template<typename InElementType>
	class TManager : public INonCopyable
	{
	public:
		TManager() = default;

		static TReference<InElementType> GetInstance()
		{
			return Instance;
		}

	protected:
		static inline InElementType* Instance = nullptr;

		virtual void Init() = 0;
		virtual void Dispose() = 0;
	};
}
