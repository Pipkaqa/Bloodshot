#pragma once

#include "SystemStorage.h"
#include "Utility/ISingleton.h"

namespace Bloodshot
{
	class SystemManager final : public ISingleton<SystemManager>
	{
		CORE_MODULE;
		ECS_MODULE;

		using ISingleton::Create;

		void Init() override;
		void Dispose() override;

		template<typename T, typename... Args>
			requires (std::is_base_of_v<ISystem, T>)
		T* AddSystem(SystemStorage* storage, Args&&... args)
		{
			const auto systemTypeID = T::s_TypeID;

			auto& systems = storage->m_Systems;

			auto it = systems.find(systemTypeID);
			
			BS_ASSERT(it == systems.end() || it->second, "An attempt to add already existing system");

			ISystem* systemInterface = new T(std::forward<Args>(args)...);

			storage->Store(systemInterface, systemTypeID);

			return FastCast<T*>(systemInterface);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		void RemoveSystem(SystemStorage* storage)
		{
			const auto systemTypeID = T::s_TypeID;

			auto& systems = storage->m_Systems;

			auto it = systems.find(systemTypeID);

			auto systemInterface = it->second;

			BS_ASSERT(it != systems.end() && systemInterface, "An attempt to remove not existing system");

			storage->Unstore(systemInterface);

			delete systemInterface;
		}

		void RemoveAllSystems(SystemStorage* storage);

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		T* GetSystem(const SystemStorage* storage) const
		{
			const auto typeID = T::s_TypeID;

			auto it = storage->m_Systems.find(typeID);

			BS_ASSERT(it != storage->m_Systems.end(), "An attempt to get not existing system");

			return FastCast<T*>(it->second);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		void EnableSystem(const SystemStorage* storage)
		{
			const auto typeID = T::s_TypeID;

			auto it = storage->m_Systems.find(typeID);

			BS_ASSERT(it != storage->m_Systems.end(), "An attempt to enable not existing system");

			it->second->m_Enabled = true;
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		void DisableSystem(const SystemStorage* storage)
		{
			const auto typeID = T::s_TypeID;

			auto it = storage->m_Systems.find(typeID);

			BS_ASSERT(it != storage->m_Systems.end(), "An attempt to disable not existing system");

			it->second->m_Enabled = false;
		}
	};
}
