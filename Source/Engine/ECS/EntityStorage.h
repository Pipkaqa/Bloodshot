#pragma once

#include "IEntity.h"

namespace Bloodshot
{
	class Scene;

	class EntityStorage final
	{
		ECS_MODULE;

	public:
		EntityStorage(Scene* context);
		~EntityStorage();

	private:
		Scene* m_Context;

		std::vector<IEntity*> m_Entities;

		void Store(IEntity* entityInterface);
		void Unstore(EntityID entityID);
	};
}
