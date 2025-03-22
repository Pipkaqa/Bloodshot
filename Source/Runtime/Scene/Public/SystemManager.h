#pragma once

#include "Core.h"

#include "System.h"

namespace Bloodshot
{
	class FSystemManager final
	{
		friend class Private::Launch::IEngineContext;
		friend class FScene;

	public:
		using FSystemArray = TArray<ISystem*>;

		NODISCARD static FSystemManager& GetInstance();

		template<IsSystem T, typename... ArgTypes>
		static T* AddSystem(ArgTypes&&... Args)
		{
			BS_PROFILE_FUNCTION();
			FSystemArray& Systems = GetInstance().Systems;

			if (T* System; Contains<T>(System))
			{
				BS_LOG(Error, "Trying to add already existing system");
				return System;
			}

			T* const System = NewObject<T>(std::forward<ArgTypes>(Args)...);
			Systems.EmplaceBack(System);
			return System;
		}

		template<IsSystem T>
		static void RemoveSystem()
		{
			BS_PROFILE_FUNCTION();
			T* System;
			size_t Index;

			if (!Contains<T>(System, &Index))
			{
				BS_LOG(Error, "Trying to remove not existing System");
				return;
			}

			DeleteObject(System->GetObject());
			GetInstance().Systems[Index] = nullptr;
		}

		static void RemoveAllSystems();

		template<IsSystem T>
		NODISCARD static T* GetSystem()
		{
			BS_PROFILE_FUNCTION();

			if (T* System; !Contains<T>(System))
			{
				BS_LOG(Error, "Trying to get not existing system");
				return nullptr;
			}
			else
			{
				return System;
			}
		}

		template<IsSystem T>
		static void EnableSystem()
		{
			BS_PROFILE_FUNCTION();

			if (T* System; !Contains(System))
			{
				BS_LOG(Error, "Trying to enable not existing system");
				return;
			}
			else
			{
				System->bEnabled = true;
			}
		}

		template<IsSystem T>
		static void DisableSystem()
		{
			BS_PROFILE_FUNCTION();

			if (T* System; !Contains(System))
			{
				BS_LOG(Error, "Trying to disable not existing system");
				return;
			}
			else
			{
				System->bEnabled = false;
			}
		}

	private:
		FSystemManager() {}

		FSystemArray Systems;

		NODISCARD FORCEINLINE FSystemArray& GetSystems()
		{
			return Systems;
		}

		template<IsSystem T>
		NODISCARD static bool Contains(T* OutSystem, size_t* const OutIndex = nullptr)
		{
			BS_PROFILE_FUNCTION();
			return GetInstance().Systems.FindByClass<T>(&OutSystem, OutIndex);
		}
	};
}
