#include "Framework.h"

using namespace Bloodshot::Test;

static void CreateTests()
{
	//FEntity* Entity = nullptr;
	//IComponent* Component = nullptr;
	//ISystem* System = nullptr;
	//
	//std::function<void()> EntityDestroyFunction;
	//std::function<void()> EntityRemoveAllComponentsFunction;
	//
	//auto CreateEntity = [&EntityDestroyFunction, &EntityRemoveAllComponentsFunction](const int ExpectedInstanceID) -> FEntity*
	//	{
	//		FEntity* Entity = Assert::ExpectSuccess(IECS::Instantiate);;
	//		Assert::AreEqual(ExpectedInstanceID, Entity->GetInstanceID());
	//		EntityDestroyFunction = Bind(&FEntity::Destroy, Entity);
	//		EntityRemoveAllComponentsFunction = Bind(&FEntity::RemoveAllComponents, Entity);
	//		return Entity;
	//	};
	//
	//Entity = CreateEntity(0);
	//
	//Assert::ExpectSuccess(EntityDestroyFunction);
	//Assert::ExpectError(EntityDestroyFunction);
	//Assert::ExpectError(EntityDestroyFunction);
	//Assert::ExpectError(EntityDestroyFunction);
	//
	//Entity = CreateEntity(0);
	//
	//Assert::ExpectSuccess(EntityDestroyFunction);
	//Assert::ExpectError(EntityDestroyFunction);
	//Assert::ExpectError(EntityDestroyFunction);
	//Assert::ExpectError(EntityDestroyFunction);
	//
	//Entity = CreateEntity(0);
	//
	//Assert::AreEqual(1, Assert::ExpectSuccess(IECS::AddComponent<FTestComponent>, Entity)->GetInstanceID());
	//Assert::ExpectSuccess(IECS::RemoveComponent<FTestComponent>, Entity);
	//Assert::ExpectSuccess(IECS::RemoveAllComponents, Entity);
	//Assert::AreEqual(nullptr, Assert::ExpectError(IECS::GetComponent<FTestComponent>, Entity));
	//Assert::AreEqual(false, Assert::ExpectSuccess(IECS::HasComponent<FTestComponent>, Entity));
	//
	//Assert::ExpectSuccess(EntityDestroyFunction);
	//
	//Assert::AreEqual(nullptr, Assert::ExpectError(IECS::AddComponent<FTestComponent>, Entity));
	//Assert::ExpectError(IECS::RemoveComponent<FTestComponent>, Entity);
	//Assert::ExpectError(IECS::RemoveAllComponents, Entity);
	//Assert::AreEqual(nullptr, Assert::ExpectError(IECS::GetComponent<FTestComponent>, Entity));
	//Assert::AreEqual(false, Assert::ExpectError(IECS::HasComponent<FTestComponent>, Entity));
	//
	//Entity = CreateEntity(0);
	//Entity = CreateEntity(1);
	//
	//Assert::AreEqual(2, Assert::ExpectSuccess(IECS::AddComponent<FTestComponent>, Entity)->GetInstanceID());
	//Assert::AreEqual(2, Assert::ExpectError(IECS::AddComponent<FTestComponent>, Entity)->GetInstanceID());
	//Assert::ExpectSuccess(IECS::RemoveComponent<FTestComponent>, Entity);
	//Assert::ExpectSuccess(IECS::RemoveAllComponents, Entity);
	//Assert::AreEqual(nullptr, Assert::ExpectError(IECS::GetComponent<FTestComponent>, Entity));
	//Assert::AreEqual(false, Assert::ExpectSuccess(IECS::HasComponent<FTestComponent>, Entity));
	//
	//System = Assert::ExpectSuccess(IECS::AddSystem<FTestSystem>);
	//
	//Assert::AreEqual(0, System->GetInstanceID());
	//Assert::AreEqual(0, Assert::ExpectError(IECS::AddSystem<FTestSystem>)->GetInstanceID());
	//Assert::AreEqual(0, Assert::ExpectError(IECS::AddSystem<FTestSystem>)->GetInstanceID());
	//Assert::AreEqual(0, Assert::ExpectError(IECS::AddSystem<FTestSystem>)->GetInstanceID());
	//Assert::ExpectSuccess(IECS::RemoveSystem<FTestSystem>);
	//Assert::ExpectError(IECS::RemoveSystem<FTestSystem>);
	//Assert::ExpectSuccess(IECS::RemoveAllSystems);
	//Assert::ExpectError(IECS::RemoveSystem<FTestSystem>);
	//
	//Entity = CreateEntity(2);
	
	//Assert::ExpectSuccess(IECS::AddComponent<FCameraComponent>, );

	const glm::vec3 Positions[] =
	{
		//front face
		glm::vec3(-0.5f,-0.5f,-0.5f),
		glm::vec3(-0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,0.5f,-0.5f),
		glm::vec3(0.5f,-0.5f,-0.5f),

		//back face
		glm::vec3(0.5f,-0.5f,0.5f),
		glm::vec3(0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,0.5f,0.5f),
		glm::vec3(-0.5f,-0.5f,0.5f)
	};

	const glm::vec3 Colors[] =
	{
		glm::vec3(0,0,0),
		glm::vec3(0,1,0),
		glm::vec3(1,0,0),
		glm::vec3(1,1,0)
	};

	const std::vector<uint32_t>& Indices =
	{
		//front
		0,1,2,
		2,3,0,

		//back
		4,5,6,
		6,7,4,

		//top
		8,9,10,
		10,11,8,

		//bottom
		12,13,14,
		14,15,12,

		//right
		16,17,18,
		18,19,16,

		//left
		20,21,22,
		22,23,20
	};

	const std::vector<FVertex>& Vertices =
	{
		{ Positions[0], Colors[1] },
		{ Positions[1], Colors[0] },
		{ Positions[2], Colors[2] },
		{ Positions[3], Colors[3] },

		//back face	    
		{ Positions[4], Colors[1] },
		{ Positions[5], Colors[0] },
		{ Positions[6], Colors[2] },
		{ Positions[7], Colors[3] },

		//top face	    
		{ Positions[1], Colors[1] },
		{ Positions[6], Colors[0] },
		{ Positions[5], Colors[2] },
		{ Positions[2], Colors[3] },

		//bottom face   
		{ Positions[7], Colors[1] },
		{ Positions[0], Colors[0] },
		{ Positions[3], Colors[2] },
		{ Positions[4], Colors[3] },

		//right face    
		{ Positions[3], Colors[1] },
		{ Positions[2], Colors[0] },
		{ Positions[5], Colors[2] },
		{ Positions[4], Colors[3] },

		//left face	    
		{ Positions[7], Colors[1] },
		{ Positions[6], Colors[0] },
		{ Positions[1], Colors[2] },
		{ Positions[0], Colors[3] }
	};

	FEntity* const Mesh = IECS::Instantiate();

	IECS::AddComponent<FMeshComponent>(Mesh, Vertices, Indices);

	FEntity* const Camera = IECS::Instantiate();

	IECS::AddComponent<FCameraComponent>(Camera, 45.f, 1920.f / 1080.f, 0.1f, 100.f);
	//IECS::GetComponent<FTransformComponent>(Camera)->Position = {0, 0, 0};
	//IECS::GetComponent<FTransformComponent>(Camera)->Rotation = {0, 0, 0};
}

int main(int Argc, char** Argv)
{
	{
		FTestFramework Framework;

		Framework.Init();
		Framework.SetTestScene();
		CreateTests();
		Framework.BeginPlay();
		Framework.StartLoop();
		Framework.EndPlay();
		Framework.Dispose();
	}

	FTestFramework::WriteAllocationsInfo();
}
