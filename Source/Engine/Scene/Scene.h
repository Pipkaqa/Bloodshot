#pragma once

#include "ECS/ComponentStorage.h"
#include "ECS/EntityStorage.h"
#include "ECS/SystemStorage.h"
#include "Utility/TypeInfo.h"

namespace Bloodshot
{
	using SceneID = InstanceID;
	using SceneTypeID = TypeID;

	class Renderer;
	class Window;

	class Scene abstract
	{
		ECS_PART;

	public:
		NODISCARD FORCEINLINE const char* GetTypeName() const noexcept
		{
			static const char* typeName{typeid(*this).name()};
			return typeName;
		}

		NODISCARD FORCEINLINE SceneID GetUniqueID() const noexcept
		{
			return m_UniqueID;
		}

	protected:
		virtual void BeginPlay() {}
		virtual void EndPlay() {}

	private:
		SceneID m_UniqueID = 0;

		UniquePointer<EntityStorage> m_EntityStorage = CreateUniquePointer<EntityStorage>(this);
		UniquePointer<ComponentStorage> m_ComponentStorage = CreateUniquePointer<ComponentStorage>(this);
		UniquePointer<SystemStorage> m_SystemStorage = CreateUniquePointer<SystemStorage>(this);

		void BeginSimulation();
		void EndSimulation();

		void InternalBeginPlay();
		void InternalEndPlay();

		void InternalUpdate(float deltaTime, Renderer* renderer, Window* window);
	};
}
