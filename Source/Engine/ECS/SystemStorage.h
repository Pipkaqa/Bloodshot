#pragma once

#include "ISystem.h"

namespace Bloodshot
{
	class Scene;

	class SystemStorage final
	{
		ECS_MODULE;

	public:
		SystemStorage(Scene* context);
		~SystemStorage();

	private:
		Scene* m_Context;

		std::unordered_map<SystemTypeID, ISystem*> m_Systems;
		std::list<ISystem*> m_SystemWorkOrder;

		void Store(ISystem* systemInterface, SystemTypeID systemTypeID);
		void Unstore(ISystem* systemInterface);
	};
}
