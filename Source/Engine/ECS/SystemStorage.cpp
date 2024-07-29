#include "SystemStorage.h"

#include "Debug/Logger.h"
#include "Scene/Scene.h"

namespace Bloodshot
{
	SystemStorage::SystemStorage(Scene* context)
		: m_Context(context)
	{
		BS_DEBUG("Creating system storage on scene of type [{0}]...", context->GetTypeName());
	}

	SystemStorage::~SystemStorage()
	{
		BS_DEBUG("Destroying system storage on scene of type [{0}]...", m_Context->GetTypeName());
	}

	void SystemStorage::Store(ISystem* systemInterface, SystemTypeID systemTypeID)
	{
		systemInterface->m_UniqueID = m_Systems.size();

		m_Systems[systemTypeID] = systemInterface;

		m_SystemWorkOrder.push_back(systemInterface);
	}

	void SystemStorage::Unstore(ISystem* systemInterface)
	{
		const auto systemTypeID = systemInterface->GetTypeID();

		m_Systems[systemTypeID] = nullptr;

		auto it = std::find(m_SystemWorkOrder.begin(), m_SystemWorkOrder.end(), systemInterface);

		m_SystemWorkOrder.erase(it);
	}
}
