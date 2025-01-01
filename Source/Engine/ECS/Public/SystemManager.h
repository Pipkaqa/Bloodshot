#pragma once

#include "AssertionMacros.h"
#include "Casts.h"
#include "Platform.h"
#include "Profiling/ProfilingMacros.h"
#include "System.h"
#include "Templates/Singleton.h" 
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	template<typename T>
	concept CIsSystem = std::is_base_of_v<ISystem, T>;

	class FSystemManager final : public TSingleton<FSystemManager>
	{
		friend class FScene;

	public:
		FSystemManager();

		static inline size_t SystemStorageGrow = 64;

		std::vector<TReference<ISystem>> SystemVec;

		virtual void Init() override;
		virtual void Dispose() override;

		template<CIsSystem T, typename... ArgTypes>
		static TReference<T> AddSystem(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to add already existing System");
				return ReinterpretCast<T*>(Instance->SystemVec[SystemTypeID]);
			}

			ISystem* const System = new T(std::forward<ArgTypes>(Args)...);

			System->InstanceID = Store(System, SystemTypeID);
			System->TypeID = SystemTypeID;
			System->Info = {sizeof(T), TTypeInfo<T>::GetTypeName()};

			return ReinterpretCast<TReference<T>>(System);
		}

		template<CIsSystem T>
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to remove not existing System");
				return;
			}

			TReference<ISystem> System = Instance->SystemVec[SystemTypeID];

			Unstore(SystemTypeID);

			delete System;
		}

		static void RemoveAllSystems();

		template<CIsSystem T>
		NODISCARD static TReference<T> GetSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to get not existing System");
				return nullptr;
			}

			return ReinterpretCast<TReference<T>>(Instance->SystemVec[SystemTypeID]);
		}

		template<CIsSystem T>
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to enable not existing System");
				return;
			}

			TReference<ISystem> System = Instance->SystemVec[SystemTypeID];

			System->bEnabled = true;
		}

		template<CIsSystem T>
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to disable not existing System");
				return;
			}

			TReference<ISystem> System = Instance->SystemVec[SystemTypeID];

			System->bEnabled = false;
		}

	private:
		static std::vector<TReference<ISystem>>& GetSystems();

		NODISCARD static bool Contains(const TypeID_t SystemTypeID);

		NODISCARD static InstanceID_t Store(TReference<ISystem> System, const TypeID_t SystemTypeID);

		static void Unstore(const TypeID_t SystemTypeID);
	};
}
