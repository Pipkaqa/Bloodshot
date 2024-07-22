#pragma once

#include "ISystem.h"
#include "Memory/LinearAllocator.h"
#include "Utility/ISingleton.h"

#include "SystemStorage.h"
#include <unordered_map>

namespace Bloodshot
{
	class SystemManager final : public ISingleton<SystemManager>
	{
		OWNED_BY_CORE;
		ECS_PART;

	public:
		template<typename T, typename... Args>
			requires (std::is_base_of_v<ISystem, T>)
		T* AddSystem(SystemStorage* storage, Args&&... args)
		{
			const auto systemTypeID = T::s_TypeID;

			auto it = storage->m_Systems.find(systemTypeID);

			FL_CORE_ASSERT(it == storage->m_Systems.end(), "An attempt to add system that already exists");

			auto memory = m_Allocator->Allocate(sizeof(T));

			ISystem* systemInterface = new(memory) T(std::forward<Args>(args)...);

			storage->Store(systemInterface, systemTypeID);

			return FastCast<T*>(systemInterface);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		T* GetSystem(SystemStorage* storage) const
		{
			const auto typeID = T::s_TypeID;

			auto it = storage->m_Systems.find(typeID);

			FL_CORE_ASSERT(it != storage->m_Systems.end(), "An attempt to get system that not exists");

			return FastCast<T*>(it->second);
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		void EnableSystem(SystemStorage* storage)
		{
			const auto typeID = T::s_TypeID;

			auto it = storage->m_Systems.find(typeID);

			FL_CORE_ASSERT(it != storage->m_Systems.end(), "An attempt to enable system that not exists");

			it->second->m_Enabled = true;
		}

		template<typename T>
			requires (std::is_base_of_v<ISystem, T>)
		void DisableSystem(SystemStorage* storage)
		{
			const auto typeID = T::s_TypeID;

			auto it = storage->m_Systems.find(typeID);

			FL_CORE_ASSERT(it != storage->m_Systems.end(), "An attempt to disable system that not exists");

			it->second->m_Enabled = false;
		}

	private:
		UniquePointer<LinearAllocator> m_Allocator = CreateUniquePointer<LinearAllocator>();

		void Init() override;
		void Dispose() override;

		friend class Scene;
		friend class ECS;
	};
}
