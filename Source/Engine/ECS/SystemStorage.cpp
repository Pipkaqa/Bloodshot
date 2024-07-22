#include "SystemStorage.h"

#include "Debug/Logger.h"
#include "Scene/Scene.h"

namespace Bloodshot
{
	SystemStorage::SystemStorage(Scene* context)
		: m_Context(context)
	{
		FL_CORE_DEBUG("Creating system storage on scene of type [{0}]...", context->GetTypeName());
	}

	SystemStorage::~SystemStorage()
	{
		m_Context = nullptr;

		m_SystemWorkOrder.clear();
		m_Systems.clear();
	}

	void SystemStorage::Store(ISystem* systemInterface, SystemTypeID systemTypeID)
	{
		systemInterface->m_UniqueID = m_Systems.size();

		m_Systems[systemTypeID] = systemInterface;

		m_SystemWorkOrder.push_back(systemInterface);
	}

	void SystemStorage::Unstore()
	{
		//TODO: if linear allocator be an bump allocator
		throw std::logic_error("Not Implemented Exception");
	}

	void SystemStorage::Dispose()
	{
		FL_CORE_DEBUG("Destroying system storage on scene of type [{0}]...", m_Context->GetTypeName());

		for (auto& [id, system] : m_Systems)
		{
			if (!system) continue;

			FL_CORE_TRACE("Destroying system of type [{0}]...", system->GetTypeName());

			system->EndPlay();

			system = nullptr;
		}
	}
}
