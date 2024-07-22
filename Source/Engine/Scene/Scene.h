#pragma once

#include "Utility/TypeInfo.h"
#include <ECS/ComponentStorage.h>
#include <ECS/EntityStorage.h>
#include <ECS/SystemStorage.h>

namespace Bloodshot
{
	using SceneID = InstanceID;
	using SceneTypeID = TypeID;

	class Renderer;

	class Scene abstract
	{
	public:
		Scene();

		void operator delete(void* ptr) = delete;
		void operator delete[](void* ptr) = delete;

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

		UniquePointer<EntityStorage> m_EntityStorage;
		UniquePointer<ComponentStorage> m_ComponentStorage;
		UniquePointer<SystemStorage> m_SystemStorage;

		void BeginSimulation();
		void EndSimulation();

		void InternalBeginPlay();
		void InternalEndPlay();

		void InternalUpdate(float deltaTime, Renderer* renderer);

		void Dispose();

		friend class SceneManager;
		friend class ECS;
	};
}
