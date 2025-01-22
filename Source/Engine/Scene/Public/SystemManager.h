#pragma once

#include "Core.h"

#include "System.h"

namespace Bloodshot
{
	template<typename T>
	concept IsSystem = std::is_base_of_v<ISystem, T>;

	class FSystemManager final : public TSingleton<FSystemManager>
	{
		friend class FScene;

	public:
		FSystemManager();

		using FSystemVector = TVector<TUniquePtr<ISystem>>;

		static inline size_t SystemStorageGrow = 64;

		FSystemVector Systems;

		virtual void Init() override;
		virtual void Dispose() override;

		template<IsSystem T, typename... ArgTypes>
		static TReference<T> AddSystem(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to add already existing System");
				return Instance->Systems.at(SystemTypeID).GetReference().As<T>();
			}

			TUniquePtr<ISystem> System = TUniquePtr<ISystem>(new T(std::forward<ArgTypes>(Args)...));
			TReference<ISystem> SystemReference = System.GetReference();

			SystemReference->TypeID = SystemTypeID;
			SystemReference->InstanceID = Store(std::move(System), SystemTypeID);
			//SystemReference->Info = {sizeof(T), TTypeInfo<T>::GetTypeName()};

			return SystemReference.As<T>();
		}

		template<IsSystem T>
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to remove not existing System");
				return;
			}

			Instance->Systems.at(SystemTypeID).Reset();

			Unstore(SystemTypeID);
		}

		static void RemoveAllSystems();

		template<IsSystem T>
		NODISCARD static TReference<T> GetSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to get not existing System");
				return nullptr;
			}

			return Instance->Systems.at(SystemTypeID).GetReference().As<T>();
		}

		template<IsSystem T>
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to enable not existing System");
				return;
			}

			Instance->Systems.at(SystemTypeID)->bEnabled = true;
		}

		template<IsSystem T>
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to disable not existing System");
				return;
			}

			Instance->Systems.at(SystemTypeID)->bEnabled = false;
		}

	private:
		static FSystemVector& GetSystems();

		NODISCARD static bool Contains(const TypeID_t SystemTypeID);

		NODISCARD static InstanceID_t Store(TUniquePtr<ISystem>&& System, const TypeID_t SystemTypeID);

		static void Unstore(const TypeID_t SystemTypeID);
	};
}
