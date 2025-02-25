#pragma once

#include "Core.h"

#include "ComponentManager.h"

namespace Bloodshot
{
	class FTransformComponent;

	BSCLASS();
	class FEntity final : public IObject
	{
		friend class FEntityManager;
		friend class FComponentManager;

		GENERATED_BODY();

	public:
		BSPROPERTY(Serialized);
		FString Name = "Entity";

		BSPROPERTY(Serialized, Replicated);
		bool bActive = true;

		NODISCARD FORCEINLINE TReference<FTransformComponent> GetTransformComponent() noexcept
		{
			return TransformComponent;
		}

		template<IsComponent T>
		FORCEINLINE TReference<T> AddComponent()
		{
			return FComponentManager::AddComponent<T>(this);
		}

		template<IsComponent T>
		FORCEINLINE void RemoveComponent()
		{
			FComponentManager::RemoveComponent<T>(this);
		}

		FORCEINLINE void RemoveAllComponents()
		{
			FComponentManager::RemoveAllComponents(this);
		}

		template<IsComponent T>
		NODISCARD FORCEINLINE TReference<T> GetComponent() const
		{
			return FComponentManager::GetComponent<T>((FEntity*)this);
		}

		template<IsComponent T>
		NODISCARD FORCEINLINE TReference<T> HasComponent() const
		{
			return FComponentManager::HasComponent<T>((FEntity*)this);
		}

		void Destroy();

	private:
		TReference<FTransformComponent> TransformComponent;
	};
}
