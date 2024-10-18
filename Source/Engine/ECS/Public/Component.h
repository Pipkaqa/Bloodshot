#pragma once

#include "Platform.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class FEntity;

	struct FComponentInfo final
	{
		size_t Size = 0;
		const char* TypeName = "Unknown";
	};

	class IComponent abstract
	{
		friend class FComponentManager;

	public:
		virtual ~IComponent() {}

		void operator delete(void* Block) = delete;
		void operator delete[](void* Block) = delete;

		bool bActive = true;

		NODISCARD FORCEINLINE InstanceID_t GetInstanceID() const noexcept
		{
			return InstanceID;
		}

		NODISCARD FORCEINLINE TypeID_t GetTypeID() const noexcept
		{
			return TypeID;
		}

		NODISCARD FORCEINLINE FComponentInfo GetInfo() const noexcept
		{
			return Info;
		}

		NODISCARD FORCEINLINE FEntity* GetOwner() noexcept
		{
			return Owner;
		}

	protected:
		InstanceID_t InstanceID = 0;
		TypeID_t TypeID = 0;
		FComponentInfo Info = {};
		FEntity* Owner = nullptr;

		virtual void BeginPlay() {}
		virtual void EndPlay() {}
	};
}
