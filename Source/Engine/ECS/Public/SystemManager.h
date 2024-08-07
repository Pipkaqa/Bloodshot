#pragma once

#include "AssertionMacros.h"
#include "Casts.h"
#include "ISystem.h"
#include "Platform.h"
#include "SystemStorage.h"
#include "Templates/Singleton.h" 
#include "Templates/SmartPointers.h"
#include "Templates/TypeInfo.h"

namespace Bloodshot
{
	class FSystemManager final : public TSingleton<FSystemManager>
	{
		friend struct IECS;

	public:
		virtual void Init() override;
		virtual void Dispose() override;

	private:
		template<typename T, typename... ArgTypes>
			requires (std::is_base_of_v<ISystem, T>)
		NODISCARD static T* AddSystem(TUniquePtr<FSystemStorage>& Storage, ArgTypes&&... Args)
		{
			const TypeID_t SystemTypeID = T::TypeID;

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(!Storage->Contains(SystemTypeID), "Attempting to add already existing system");
			if (Storage->Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to add already existing System");
				return ReinterpretCast<T*>(Storage->Get(SystemTypeID));
			}

			ISystem* const System = new T(std::forward<ArgTypes>(Args)...);

			Storage->Store(System, SystemTypeID);

			return ReinterpretCast<T*>(System);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void RemoveSystem(TUniquePtr<FSystemStorage>& Storage)
		{
			const TypeID_t SystemTypeID = T::TypeID;

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(Storage->Contains(SystemTypeID), "Attempting to remove not existing system");
			if (!Storage->Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to remove not existing System");
				return;
			}

			ISystem* const System = Storage->Get(SystemTypeID);

			Storage->Unstore(SystemTypeID);

			delete System;
		}

		static void RemoveAllSystems(TUniquePtr<FSystemStorage>& storage);

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		NODISCARD static T* GetSystem(const TUniquePtr<FSystemStorage>& Storage)
		{
			const TypeID_t SystemTypeID = T::TypeID;

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(Storage->Contains(SystemTypeID), "Attempting to get not existing system");
			if (!Storage->Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to get not existing System");
				return nullptr;
			}

			return ReinterpretCast<T*>(Storage->Get(SystemTypeID));
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void EnableSystem(const TUniquePtr<FSystemStorage>& Storage)
		{
			const TypeID_t SystemTypeID = T::TypeID;

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(Storage->Contains(SystemTypeID), "Attempting to enable not existing system");
			if (!Storage->Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to enable not existing System");
				return;
			}

			ISystem* const System = Storage->Get(SystemTypeID);

			System->bEnabled = true;
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		static void DisableSystem(const TUniquePtr<FSystemStorage>& Storage)
		{
			const TypeID_t SystemTypeID = T::TypeID;

			// BSNOTE: Assert -> Error (06.08.2024)
			//BS_ASSERT(Storage->Contains(SystemTypeID), "Attempting to disable not existing System");
			if (!Storage->Contains(SystemTypeID))
			{
				BS_LOG(Error, "Attempting to disable not existing System");
				return;
			}

			ISystem* const System = Storage->Get(SystemTypeID);

			System->bEnabled = false;
		}
	};
}
