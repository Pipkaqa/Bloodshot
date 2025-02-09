#pragma once

#include "Core.h"

#include "Handle.h"
#include "System.h"

namespace Bloodshot
{
	template<typename T>
	concept IsSystem = std::is_base_of_v<ISystem, T>;

	class FSystemManager final : public TManager<FSystemManager>
	{
		friend class IEngineContext;
		friend class FScene;

	public:
		using FSystemArray = TArray<TReference<ISystem>>;

		static inline size_t SystemStorageGrow = 64;

		template<IsSystem T, typename... ArgTypes>
		static TReference<T> AddSystem(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			const FTypeID SystemTypeID = FTypeID::Get<ISystem, T>();

			if (Contains(SystemTypeID))
			{
				BS_LOG(Error, "Trying to add already existing System");
				return Instance->Systems[SystemTypeID].GetReference().As<T>();
			}

			TReference<ISystem> System = NewObject<T>(std::forward<ArgTypes>(Args)...);

			System->InstanceID = Store(System, SystemTypeID);
			System->TypeID = SystemTypeID;

			return System.As<T>();
		}

		template<IsSystem T>
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();

			const FTypeID SystemTypeID = FTypeID::Get<ISystem, T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Trying to remove not existing System");
				return;
			}

			Instance->Systems[SystemTypeID].Reset();

			Unstore(SystemTypeID);
		}

		static void RemoveAllSystems();

		template<IsSystem T>
		NODISCARD static TReference<T> GetSystem()
		{
			BS_PROFILE_FUNCTION();

			const FTypeID SystemTypeID = FTypeID::Get<ISystem, T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Trying to get not existing System");
				return nullptr;
			}

			return Instance->Systems[SystemTypeID].GetReference().As<T>();
		}

		template<IsSystem T>
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			const FTypeID SystemTypeID = FTypeID::Get<ISystem, T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Trying to enable not existing System");
				return;
			}

			Instance->Systems[SystemTypeID]->bEnabled = true;
		}

		template<IsSystem T>
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			const FTypeID SystemTypeID = FTypeID::Get<ISystem, T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Trying to disable not existing System");
				return;
			}

			Instance->Systems[SystemTypeID]->bEnabled = false;
		}

	private:
		FSystemManager();

		FSystemArray Systems;

		virtual void Init() override;
		virtual void Dispose() override;

		static FSystemArray& GetSystems();

		NODISCARD static bool Contains(const FTypeID SystemTypeID);

		NODISCARD static FInstanceID Store(TReference<ISystem> System, const FTypeID SystemTypeID);

		static void Unstore(const FTypeID SystemTypeID);
	};
}
