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
		FL_CORE_DEBUG("Destroying system storage on scene of type [{0}]...", m_Context->GetTypeName());
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
}
