#include "Framework.h"

#include <AssetLoader.h>
#include <CameraComponent.h>
#include <Component.h>
#include <ComponentManager.h>
#include <Entity.h>
#include <EntityManager.h>
#include <Logging/Logger.h>
#include <Logging/LoggingMacros.h>
#include <MathLibrary.h>
#include <Memory/Memory.h>
#include <MeshComponent.h>
#include <Profiling/Profiler.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <System.h>
#include <SystemManager.h>
#include <Templates/SmartPointers.h>
#include <TransformComponent.h>
#include <Util/Util.h>

namespace
{
	class FControllerComponent final : public Bloodshot::IComponent
	{
	};

	class FControllerSystem final : public Bloodshot::ISystem
	{
		virtual void FixedTick() override
		{
			using namespace Bloodshot;
			using ControllerComponentIterator = TComponentIterator<FControllerComponent>;

			ControllerComponentIterator ControllerComponentBeginIterator = FComponentManager::Begin<FControllerComponent>();
			ControllerComponentIterator ControllerComponentEndIterator = FComponentManager::End<FControllerComponent>();

			for (; ControllerComponentBeginIterator != ControllerComponentEndIterator; ++ControllerComponentBeginIterator)
			{
			}
		}

		virtual void Tick(float DeltaTime) override
		{
			using namespace Bloodshot;
			using MeshComponentIterator = TComponentIterator<FMeshComponent>;

			MeshComponentIterator MeshComponentBeginIterator = FComponentManager::Begin<FMeshComponent>();
			MeshComponentIterator MeshComponentEndIterator = FComponentManager::End<FMeshComponent>();

			for (; MeshComponentBeginIterator != MeshComponentEndIterator; ++MeshComponentBeginIterator)
			{
			}
		}
	};

	class FTestComponent : public Bloodshot::IComponent
	{
	};

	class FTestSystem : public Bloodshot::ISystem
	{
	};

	struct FEntityWrapper final
	{
		Bloodshot::TReference<Bloodshot::FEntity> Entity = nullptr;

		std::function<void()> Destroy;
		std::function<void()> RemoveAllComponents;
	};

	static FEntityWrapper CreateAndTestEntity(const int ExpectedInstanceID)
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		TReference<FEntity> Entity = IAssert::ExpectSuccess(FEntityManager::Instantiate);
		IAssert::AreEqual(ExpectedInstanceID, Entity->GetInstanceID());

		FEntityWrapper Wrapper;

		Wrapper.Entity = Entity;
		Wrapper.Destroy = Bind(&FEntity::Destroy, Entity);
		Wrapper.RemoveAllComponents = Bind(&FEntity::RemoveAllComponents, Entity);

		return Wrapper;
	};

	static void EntitiesTest()
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		FEntityWrapper Entity0 = CreateAndTestEntity(0);
		IAssert::ExpectSuccess(Entity0.Destroy);
		IAssert::ExpectError(Entity0.Destroy);
		IAssert::ExpectError(Entity0.Destroy);
		IAssert::ExpectError(Entity0.Destroy);

		FEntityWrapper Entity1 = CreateAndTestEntity(0);
		IAssert::ExpectSuccess(Entity1.Destroy);
		IAssert::ExpectError(Entity1.Destroy);
		IAssert::ExpectError(Entity1.Destroy);
		IAssert::ExpectError(Entity1.Destroy);

		FEntityWrapper Entity2 = CreateAndTestEntity(0);
		IAssert::ExpectSuccess(Entity2.Destroy);

		FEntityWrapper Entity3 = CreateAndTestEntity(0);
		FEntityWrapper Entity4 = CreateAndTestEntity(1);
		FEntityWrapper Entity5 = CreateAndTestEntity(2);
		IAssert::ExpectSuccess(Entity3.Destroy);
		IAssert::ExpectSuccess(Entity4.Destroy);
		IAssert::ExpectSuccess(Entity5.Destroy);
		IAssert::ExpectError(Entity3.Destroy);
		IAssert::ExpectError(Entity4.Destroy);
		IAssert::ExpectError(Entity5.Destroy);
	}

	static void ComponentsTest()
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		IAssert::ExpectSuccess(FComponentManager::AddComponent<FControllerComponent>, FEntityManager::Instantiate());

		TReference<FEntity> Entity1 = FEntityManager::Instantiate();
		FComponentManager::GetComponent<FTransformComponent>(Entity1)->Position = {0.f, 0.f, 0.f};
		FComponentManager::GetComponent<FTransformComponent>(Entity1)->Rotation = {0.f, 180.f, 0.f};
		FComponentManager::GetComponent<FTransformComponent>(Entity1)->Scale = {1.1f, 1.1f, 1.1f};
		IAssert::ExpectError(FComponentManager::GetComponent<FControllerComponent>, Entity1);

		TReference<FEntity> Entity2 = FEntityManager::Instantiate();
		FComponentManager::GetComponent<FTransformComponent>(Entity2)->Position = {0.f, 0.f, -1.f};
		FComponentManager::GetComponent<FTransformComponent>(Entity2)->Rotation = {90.f, 0.f, 0.f};
		FComponentManager::AddComponent<FControllerComponent>(Entity2);

		IAssert::ExpectSuccess(FComponentManager::RemoveComponent<FControllerComponent>, Entity2);
		IAssert::ExpectError(FComponentManager::RemoveComponent<FControllerComponent>, Entity2);

		IAssert::ExpectSuccess(FComponentManager::RemoveAllComponents, Entity2);
		IAssert::ExpectSuccess(FComponentManager::RemoveAllComponents, Entity2);
	}

	static void SystemsTest()
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		TReference<ISystem> System = IAssert::ExpectSuccess(FSystemManager::AddSystem<FTestSystem>);

		IAssert::AreEqual(0, System->GetInstanceID());
		IAssert::AreEqual(0, IAssert::ExpectError(FSystemManager::AddSystem<FTestSystem>)->GetInstanceID());
		IAssert::AreEqual(0, IAssert::ExpectError(FSystemManager::AddSystem<FTestSystem>)->GetInstanceID());
		IAssert::AreEqual(0, IAssert::ExpectError(FSystemManager::AddSystem<FTestSystem>)->GetInstanceID());
		IAssert::ExpectSuccess(FSystemManager::RemoveSystem<FTestSystem>);
		IAssert::ExpectError(FSystemManager::RemoveSystem<FTestSystem>);
		IAssert::ExpectSuccess(FSystemManager::RemoveAllSystems);
		IAssert::ExpectError(FSystemManager::RemoveSystem<FTestSystem>);

		FSystemManager::AddSystem<FControllerSystem>();
	}

	static void ComplexTest()
	{
		using namespace Bloodshot;
		using namespace Bloodshot::Test;

		TReference<FEntity> Entity0 = FEntityManager::Instantiate();
		IAssert::AreEqual(4, IAssert::ExpectSuccess(FComponentManager::AddComponent<FTestComponent>, Entity0)->GetInstanceID());
		IAssert::ExpectSuccess(FComponentManager::RemoveComponent<FTestComponent>, Entity0);
		IAssert::ExpectSuccess(FComponentManager::RemoveAllComponents, Entity0);
		IAssert::AreEqual(nullptr, IAssert::ExpectError(FComponentManager::GetComponent<FTestComponent>, Entity0));
		IAssert::AreEqual(false, IAssert::ExpectSuccess(FComponentManager::HasComponent<FTestComponent>, Entity0));
		IAssert::ExpectSuccess(FEntityManager::Destroy, Entity0);
		IAssert::AreEqual(nullptr, IAssert::ExpectError(FComponentManager::AddComponent<FTestComponent>, Entity0));
		IAssert::ExpectError(FComponentManager::RemoveComponent<FTestComponent>, Entity0);
		IAssert::ExpectError(FComponentManager::RemoveAllComponents, Entity0);
		IAssert::AreEqual(nullptr, IAssert::ExpectError(FComponentManager::GetComponent<FTestComponent>, Entity0));
		IAssert::AreEqual(false, IAssert::ExpectError(FComponentManager::HasComponent<FTestComponent>, Entity0));

		TReference<FEntity> Entity1 = FEntityManager::Instantiate();
		IAssert::AreEqual(4, IAssert::ExpectSuccess(FComponentManager::AddComponent<FTestComponent>, Entity1)->GetInstanceID());
		IAssert::AreEqual(4, IAssert::ExpectError(FComponentManager::AddComponent<FTestComponent>, Entity1)->GetInstanceID());
		IAssert::ExpectSuccess(FComponentManager::RemoveComponent<FTestComponent>, Entity1);
		IAssert::ExpectSuccess(FComponentManager::RemoveAllComponents, Entity1);
		IAssert::AreEqual(nullptr, IAssert::ExpectError(FComponentManager::GetComponent<FTestComponent>, Entity1));
		IAssert::AreEqual(false, IAssert::ExpectSuccess(FComponentManager::HasComponent<FTestComponent>, Entity1));
	}
}

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;
	using namespace Bloodshot::Test;
	using namespace Bloodshot::Private;

#ifdef BS_LOGGING_ON
	FLogger Logger;
#endif
	{
#ifdef BS_PROFILING_ON
		FProfiler Profiler;
#endif

#ifdef BS_LOGGING_ON
		Logger.BeginSession((ELogLevel::All), EFileOpenMode::Truncate);
		BS_CHECK(Logger.IsSessionStarted(), "Logging Session not started!");
#endif
#ifdef BS_PROFILING_ON
		Profiler.BeginSession();
		BS_CHECK(Profiler.IsSessionStarted(), "Profiling Session not started!");
#endif

		FMemory Memory;
		FEngineState EngineState;
		FEngineTime EngineTime;
		FResourceManager ResourceManager;
		FEntityManager EntityManager;
		FComponentManager ComponentManager;
		FSystemManager SystemManager;
		FSceneManager SceneManager;

		EngineState.Init();
		EngineTime.Init();
		ResourceManager.Init();
		EntityManager.Init();
		ComponentManager.Init();
		SystemManager.Init();
		SceneManager.Init();

		EngineState.bRunning = true;

		SceneManager.AddScene();
		SceneManager.SetStartingScene(0);

		EntitiesTest();
		ComponentsTest();
		SystemsTest();
		ComplexTest();

		SceneManager.BeginPlay();
		EngineState.bSimulating = true;

		EngineTime.Tick();
		SceneManager.Tick(0);

		SceneManager.EndPlay();
		EngineState.bSimulating = false;

		SceneManager.Dispose();
		SystemManager.Dispose();
		ComponentManager.Dispose();
		EntityManager.Dispose();
		ResourceManager.Dispose();
		EngineTime.Dispose();
		EngineState.Dispose();

		EngineState.bRunning = false;
	}
#ifdef BS_LOGGING_ON
	const FAllocationInfo& AllocationsInfo = IAllocationLogger::GetAllocationsInfo();

	BS_LOG(Info, "Allocated:   {0}.B, {1}.MB, {2} Blocks",
		AllocationsInfo.AllocatedSize,
		AllocationsInfo.AllocatedSize >> 20ULL,
		AllocationsInfo.AllocatedBlockCount);

	BS_LOG(Info, "Deallocated: {0}.B, {1}.MB, {2} Blocks",
		AllocationsInfo.DeallocatedSize,
		AllocationsInfo.DeallocatedSize >> 20ULL,
		AllocationsInfo.DeallocatedBlockCount);

	IAllocationLogger::IsMemoryLeak() ? BS_LOG(Warning, "Memory leak detected") : BS_LOG(Info, "Memory leak not detected");
#endif
}
