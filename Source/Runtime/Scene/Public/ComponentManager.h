#pragma once

#include "Core.h"

#include "Component.h"

namespace Bloodshot
{
	template<IsComponent T>
	using TComponentIterator = Private::Object::TObjectIterator<T>;

	class FComponentManager final
	{
		friend class Private::Launch::IEngineContext;
		friend class FScene;

	public:
		NODISCARD static FComponentManager& GetInstance();
	
		template<IsComponent T>
		NODISCARD FORCEINLINE static TComponentIterator<T> CreateComponentIterator()
		{
			return Private::Object::FObjectCore::CreateObjectIterator<T>();
		}

	private:
		FComponentManager() {}
	};
}
