#pragma once

#include "AssertionMacros.h"
#include "Casts.h"
#include "Platform.h"
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
		friend struct IECS;
		friend class FScene;

	public:
		FSystemManager();

		std::vector<ISystem*> SystemVec;

		virtual void Init() override;
		virtual void Dispose() override;

	private:
		static std::vector<ISystem*>& GetSystems();

		template<CIsSystem T, typename... ArgTypes>
		NODISCARD static T* AddSystem(ArgTypes&&... Args)
		{
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

			return ReinterpretCast<T*>(System);
		}

		template<CIsSystem T>
		static void RemoveSystem()
		{
			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to remove not existing System");
				return;
			}

			ISystem* const System = Instance->SystemVec[SystemTypeID];

			Unstore(SystemTypeID);

			delete System;
		}

		static void RemoveAllSystems();

		template<CIsSystem T>
		NODISCARD static T* GetSystem()
		{
			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to get not existing System");
				return nullptr;
			}

			return ReinterpretCast<T*>(Instance->SystemVec[SystemTypeID]);
		}

		template<CIsSystem T>
		static void EnableSystem()
		{
			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to enable not existing System");
				return;
			}

			ISystem* const System = Instance->SystemVec[SystemTypeID];

			System->bEnabled = true;
		}

		template<CIsSystem T>
		static void DisableSystem()
		{
			const TypeID_t SystemTypeID = TTypeInfo<ISystem>::GetTypeID<T>();

			if (!Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to disable not existing System");
				return;
			}

			ISystem* const System = Instance->SystemVec[SystemTypeID];

			System->bEnabled = false;
		}

		NODISCARD static bool Contains(const TypeID_t SystemTypeID);

		NODISCARD static InstanceID_t Store(ISystem* const System, const TypeID_t SystemTypeID);

		static void Unstore(const TypeID_t SystemTypeID);
	};
}
