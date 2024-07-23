#pragma once

#include "Utility/Utility.h"
#include "IEntity.h"

namespace Bloodshot
{
	class Scene;

	class EntityStorage final
	{
		ECS_PART;

	public:
		EntityStorage(Scene* context);
		~EntityStorage();

	private:
		Scene* m_Context;

		std::vector<IEntity*> m_Entities;

		void Store(IEntity* entityInterface);
		void Unstore(EntityID entityID);

		friend class Scene;
	};
}
