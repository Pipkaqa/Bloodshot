#pragma once

#include "ECS/Component.h"
#include "ECS/Entity.h"
#include "ECS/System.h"
#include "Scene/Scene.h"

namespace Bloodshot::Test
{
	class TestScene : public Bloodshot::Scene
	{

	};

	class TestEntity : public Bloodshot::Entity<TestEntity>
	{

	};
	
	class TestComponent : public Bloodshot::Component<TestComponent>
	{

	};

	class TestSystem : public Bloodshot::System<TestSystem>
	{

	};
}
