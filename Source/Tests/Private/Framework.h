#include "Vertex.h"
#include <Application.h>
#include <AssertionMacros.h>
#include <ComponentManager.h>
#include <ECS.h>
#include <EngineState.h>
#include <EngineTime.h>
#include <EntityManager.h>
#include <Logging/Logger.h>
#include <Logging/LoggingMacros.h>
#include <Memory/Memory.h>
#include <MeshComponent.h>
#include <Networking.h>
#include <OpenGL/OpenGLHeader.h>
#include <OpenGL/OpenGLRenderer.h>
#include <OpenGL/OpenGLWindow.h>
#include <Profiling/Profiler.h>
#include <Profiling/ProfilingMacros.h>
#include <Renderer.h>
#include <ResourceManager.h>
#include <SceneManager.h>
#include <SystemManager.h>
#include <Templates/SmartPointers.h>
#include <Util/Util.h>
#include <Window.h>

#include <exception>
#include <filesystem>

namespace Bloodshot::Test
{
	struct Assert abstract final
	{
		// BSTODO: Write correct implementation of ExpectSuccess(), ExpectError() functions

	public:
		template<typename FunctionType, typename... ArgTypes>
		static auto ExpectSuccess(FunctionType&& Function, ArgTypes&&... Args)
		{
			LastErrorCount = ILogger::GetErrorCount();

			if constexpr (std::is_void_v<std::invoke_result_t<decltype(Function), ArgTypes...>>)
			{
				Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount != ILogger::GetErrorCount(), Fatal, "Success Test Failed");
				return;
			}
			else
			{
				auto ReturnValue = Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount != ILogger::GetErrorCount(), Fatal, "Success Test Failed");
				return ReturnValue;
			}
		}

		template<typename FunctionType, typename... ArgTypes>
		static auto ExpectError(FunctionType&& Function, ArgTypes&&... Args)
		{
			LastErrorCount = ILogger::GetErrorCount();

			if constexpr (std::is_void_v<std::invoke_result_t<decltype(Function), ArgTypes...>>)
			{
				Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount == ILogger::GetErrorCount(), Fatal, "Success Test Failed");
				return;
			}
			else
			{
				auto ReturnValue = Function(std::forward<ArgTypes>(Args)...);
				BS_LOG_IF(LastErrorCount == ILogger::GetErrorCount(), Fatal, "Success Test Failed");
				return ReturnValue;
			}
		}

		template<typename T>
		static void AreEqual(T&& First, T&& Second)
		{
			BS_LOG_IF(First != Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T, typename U>
		static void AreEqual(T&& First, U&& Second)
		{
			BS_LOG_IF(First != Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T>
			requires std::is_pointer_v<T>
		static void AreEqual(nullptr_t First, T&& Second)
		{
			BS_LOG_IF(First != Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T>
		static void AreNotEqual(T&& First, T&& Second)
		{
			BS_LOG_IF(First == Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T, typename U>
		static void AreNotEqual(T&& First, U&& Second)
		{
			BS_LOG_IF(First == Second, Fatal, "AreEqual Test Failed");
		}

		template<typename T>
			requires std::is_pointer_v<T>
		static void AreNotEqual(nullptr_t First, T&& Second)
		{
			BS_LOG_IF(First == Second, Fatal, "AreEqual Test Failed");
		}

	private:
		static inline size_t LastErrorCount = 0;
	};

	using namespace Bloodshot;
	using namespace Bloodshot::Private;
#ifdef BS_NETWORKING_ON
	using namespace Bloodshot::Networking;
#endif

	struct FTestFramework final
	{
		size_t ErrorCount = 0;

#ifdef BS_PROFILING_ON
		FProfiler Profiler;
#endif
		FEngineState EngineState;
		FEngineTime EngineTime;
#ifdef BS_NETWORKING_ON
		FNetwork Network;
#endif
		FResourceManager ResourceManager;

		TUniquePtr<IWindow> Window = nullptr;
		TUniquePtr<IRenderer> Renderer = nullptr;

		FEntityManager EntityManager;
		FComponentManager ComponentManager;
		FSystemManager SystemManager;
		FSceneManager SceneManager;
		FApplication Application = {Window};

		static void WriteAllocationsInfo()
		{
#ifdef BS_LOGGING_ON
			const IMemory::FAllocationsInfo AllocationsInfo = IMemory::GetAllocationsInfo();

			BS_LOG(Info, "Allocated:   {0}.B, {1}.MB, {2} Blocks",
				AllocationsInfo.AllocatedSize,
				AllocationsInfo.AllocatedSize >> 20ULL,
				AllocationsInfo.AllocatedBlockCount);

			BS_LOG(Info, "Deallocated: {0}.B, {1}.MB, {2} Blocks",
				AllocationsInfo.DeallocatedSize,
				AllocationsInfo.DeallocatedSize >> 20ULL,
				AllocationsInfo.DeallocatedBlockCount);

			IMemory::IsMemoryLeak() ? BS_LOG(Warning, "Memory leak detected") : BS_LOG(Info, "Memory leak not detected");

			ILogger::EndSession();
#endif
		}

		void Init()
		{
#ifdef BS_LOGGING_ON
			ILogger::BeginSession((ELogLevel::All), EFileOpenMode::Truncate);
			BS_CHECK(ILogger::IsSessionStarted(), "Logging Session not started!");
#endif
#ifdef BS_PROFILING_ON
			Profiler.BeginSession();
			BS_CHECK(Profiler.IsSessionStarted(), "Profiling Session not started!");
#endif
			BS_LOG(Debug, "Creating the Core...");

			// BSTODO: Temporary hard-coded
			ERendererType RendererType = ERendererType::OpenGL;

			// BSTODO: Add Vulkan support
			switch (RendererType)
			{
				case ERendererType::OpenGL:
				{
					BS_LOG_IF(!(Window = MakeUnique<FOpenGLWindow>()), Fatal, "Failed to create Window");
					BS_LOG_IF(!(Renderer = MakeUnique<FOpenGLRenderer>()), Fatal, "Failed to create Renderer");

					break;
				}
				case ERendererType::Vulkan:
				{
					BS_LOG(Fatal, "Vulkan is not supported");

					break;
				}
			}

			// BSTODO: Temporary hard-coded
			IECS::EntityStorageGrow = 1024;
			IECS::ComponentStorageGrow = 1024;

			BS_LOG(Debug, "Initializing the Core...");
			{
				BS_PROFILE_RANGE("Initializing the Core");

				EngineState.Init();
				EngineTime.Init();
#ifdef BS_NETWORKING_ON
				Network.Init();
#endif
				ResourceManager.Init();
				Window->Init();
				Renderer->Init();
				EntityManager.Init();
				ComponentManager.Init();
				SystemManager.Init();
				SceneManager.Init();

				const std::string& CurrentWorkingDirectory = std::filesystem::current_path().string();

				BS_LOG(Info, "Working directory: {0}", CurrentWorkingDirectory);
				BS_LOG(Info, "Log directory: {0}\\Log", CurrentWorkingDirectory);
				BS_LOG(Info, "Resources directory: {0}\\{1}", CurrentWorkingDirectory, FResourceManager::ResourcesPath);

				EngineState.bRunning = true;
			}
		}

		void Dispose()
		{
			BS_LOG(Debug, "Destroying the Core...");
			{
				BS_PROFILE_RANGE("Dispose");

				SceneManager.Dispose();
				SystemManager.Dispose();
				ComponentManager.Dispose();
				EntityManager.Dispose();
				Renderer->Dispose();
				Window->Dispose();
				ResourceManager.Dispose();
#ifdef BS_NETWORKING_ON
				Network.Dispose();
#endif
				EngineTime.Dispose();
				EngineState.Dispose();

				EngineState.bRunning = false;
			}

#ifdef BS_PROFILING_ON
			Profiler.EndSession();
#endif
		}

		void StartLoop()
		{
			float FrameTimeAccumulation = 0.f;

			while (!Window->ShouldClose())
			{
				try
				{
					BS_PROFILE_RANGE("Main Loop Tick");

					EngineTime.Tick();

					Window->BeginFrame();
					SceneManager.Tick(IWindow::GetFrameTime(), Renderer, Window);
					Window->EndFrame();

					FrameTimeAccumulation += Window->GetFrameTime();

					if (FrameTimeAccumulation >= 1000.f)
					{
						BS_LOG(Info, "Frame Time: {0} ms", Window->GetFrameTime());
						BS_LOG(Info, "FPS: {0}", Window->GetFramesPerSecond());

						FrameTimeAccumulation = 0.f;
					}

					//break;
				}
				catch (const std::exception& Exception)
				{
					BS_LOG(Error, "Exception occurred: {0}", Exception.what());
				}
				catch (...)
				{
					BS_LOG(Error, "Undefined Exception occurred");
				}
			}
		}

		void SetTestScene()
		{
			BS_LOG(Debug, "Adding Scenes...");
			{
				BS_PROFILE_RANGE("Adding Scenes");

				SceneManager.AddScene();
				SceneManager.SetStartingScene(0);
			}
		}

		void BeginPlay()
		{
			try
			{
				BS_LOG(Debug, "Starting simulation...");
				BS_PROFILE_RANGE("Starting Simulation");

				SceneManager.BeginPlay();
				EngineState.bSimulating = true;

			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred in Startup-Block: {0}", Exception.what());
			}
			catch (...)
			{
				BS_LOG(Error, "Undefined Exception occurred in Startup-Block");
			}
		}

		void EndPlay()
		{
			try
			{
				BS_LOG(Debug, "Ending simulation...");
				BS_PROFILE_RANGE("Shutdown");

				SceneManager.EndPlay();
				EngineState.bSimulating = false;

			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred in Shutdown-Block: {0}", Exception.what());
			}
			catch (...)
			{
				BS_LOG(Error, "Undefined Exception occurred in Shutdown-Block");
			}
		}
	};

	class FTestComponent : public Bloodshot::IComponent
	{
	};

	class FTestSystem : public Bloodshot::ISystem
	{
	};
}
