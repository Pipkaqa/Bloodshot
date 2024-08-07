#pragma once

#include "ComponentStorage.h"
#include "EntityStorage.h"
#include "Platform.h"
#include "SystemStorage.h"
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class IRenderer;
	class IWindow;

	class IScene abstract
	{
	public:
		NODISCARD FORCEINLINE const char* GetTypeName() const noexcept
		{
			static const char* TypeName{typeid(*this).name()};
			return TypeName;
		}

		NODISCARD FORCEINLINE InstanceID_t GetUniqueID() const noexcept
		{
			return UniqueID;
		}

		virtual void BeginPlay() {}
		virtual void EndPlay() {}

		InstanceID_t UniqueID = 0;

		TUniquePtr<FEntityStorage> EntityStorage = MakeUnique<FEntityStorage>(this);
		TUniquePtr<FComponentStorage> ComponentStorage = MakeUnique<FComponentStorage>(this);
		TUniquePtr<FSystemStorage> SystemStorage = MakeUnique<FSystemStorage>(this);

		void BeginSimulation();
		void EndSimulation();

		void InternalBeginPlay();
		void InternalEndPlay();

		void InternalUpdate(float DeltaTime, TUniquePtr<IRenderer>& Renderer, TUniquePtr<IWindow>& Window);
	};
}
