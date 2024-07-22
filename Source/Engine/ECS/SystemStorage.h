#pragma once

#include "ISystem.h"
#include "Utility/Utility.h"

namespace Bloodshot
{
	class Scene;

	class SystemStorage final
	{
		ECS_PART;

	public:
		~SystemStorage();

	private:
		SystemStorage(Scene* context);

		Scene* m_Context;

		std::unordered_map<SystemTypeID, ISystem*> m_Systems;
		std::vector<ISystem*> m_SystemWorkOrder;

		void Store(ISystem* systemInterface, SystemTypeID systemTypeID);
		void Unstore();

		void Dispose();

		friend class Scene;
	};
}
