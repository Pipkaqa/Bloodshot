#pragma once

#include "Core.h"

#include "Entity.h"
#include "TransformComponent.h"

namespace Bloodshot
{
	class FEntityManager final
	{
		friend class Private::Launch::IEngineContext;
		friend class Private::Launch::FEngineEditorContext;
		friend class Private::Launch::FEngineGameContext;
		friend class FScene;

	public:
		using FEntityArray = TArray<FEntity*>;

		NODISCARD static FEntityManager& GetInstance();

		FORCEINLINE static FEntity* Instantiate()
		{
			BS_PROFILE_FUNCTION();
			FEntity* const Entity = NewObject<FEntity>();
			GetInstance().Entities.Emplace(Entity);
			Entity->AddComponent<FTransformComponent>();
			return Entity;
		}

		NODISCARD FORCEINLINE static FEntityArray Instantiate(const size_t Count)
		{
			BS_PROFILE_FUNCTION();
			FEntityArray Result;
			Result.Reserve(Count);
			for (size_t i = 0; i < Count; ++i)
			{
				Result.EmplaceBack(Instantiate());
			}
			return Result;
		}

		FORCEINLINE static void Instantiate(FEntityArray& OutResult, const size_t Count)
		{
			BS_PROFILE_FUNCTION();
			OutResult.Resize(Count);
			for (FEntity*& Entity : OutResult)
			{
				Entity = Instantiate();
			}
		}

		FORCEINLINE static void Destroy(FEntity* const Entity)
		{
			BS_PROFILE_FUNCTION();
			if (DestroyImpl(Entity))
			{
				GetInstance().Entities.Remove(Entity);
			}
		}

		FORCEINLINE static void Destroy(FEntityArray& InOutEntities)
		{
			BS_PROFILE_FUNCTION();
			for (FEntity* const Entity : InOutEntities)
			{
				Destroy(Entity);
			}
			InOutEntities.Clear();
		}

	private:
		FEntityManager() {}

		TSet<FEntity*> Entities;

		FORCEINLINE static bool DestroyImpl(FEntity* const Entity)
		{
			Entity->RemoveAllComponents();
			DeleteObject(Entity->GetObject());
			return true;
		}

		FORCEINLINE static void PrivateDestroy(FEntity* const Entity)
		{
			Entity->PrivateRemoveAllComponents();
			DeleteObject(Entity->GetObject());
		}

		FORCEINLINE void PrivateDestroyAllEntities()
		{
			BS_PROFILE_FUNCTION();
			for (FEntity* const Entity : Entities)
			{
				PrivateDestroy(Entity);
			}
			Entities.Clear();
		}
	};
}
