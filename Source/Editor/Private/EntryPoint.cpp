// BSTODO:
//		Event system, what handles: scene reloading, key presses, any input, window resizing, application exit, etc...
//		Add new allocators, minimize new calls, maximize cache locality
//		Inject entry point in engine

#include "EnetTemplate.h"
#include "OpenGLTemplate.h"

#include <Application.h>
#include <AssertionMacros.h>
#include <CameraComponent.h>
#include <ComponentManager.h>
#include <ECS.h>
#include <EngineState.h>
#include <EngineTime.h>
#include <EntityManager.h>
#include <Logging/Logger.h>
#include <Logging/LoggingMacros.h>
#include <Memory/Memory.h>
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
#include <Window.h>

#include <exception>
#include <filesystem>

int main(int Argc, char** Argv)
{
	OpenGLTemplate();
	return 0;

	using namespace Bloodshot;
	using namespace Bloodshot::Private;
#ifdef BS_NETWORKING_ON
	using namespace Bloodshot::Networking;
#endif
#ifdef BS_LOGGING_ON
	{
		ILogger::BeginSession((ELogLevel::All), EFileOpenMode::Truncate);
		BS_CHECK(ILogger::IsSessionStarted(), "Logging Session not started!");
		{
#endif
#ifdef BS_PROFILING_ON
			FProfiler Profiler;

			Profiler.BeginSession();

			BS_CHECK(Profiler.IsSessionStarted(), "Profiling Session not started!");
#endif
			BS_LOG(Debug, "Creating the Core...");

			FEngineState EngineState;
			FEngineTime EngineTime;
#ifdef BS_NETWORKING_ON
			FNetwork Network;
#endif
			FResourceManager ResourceManager;

			TUniquePtr<IWindow> Window = nullptr;
			TUniquePtr<IRenderer> Renderer = nullptr;

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

			FEntityManager EntityManager;
			FComponentManager ComponentManager;
			FSystemManager SystemManager;
			FSceneManager SceneManager;
			FApplication Application(Window);

			// BSTODO: Temporary hard-coded
			//IMemory::ChunksToPreAllocateByFixedAllocators = 4;
			//IMemory::BlocksPerChunkInFixedAllocators = 1024;

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

			BS_LOG(Debug, "Adding Scenes...");
			{
				BS_PROFILE_RANGE("Adding Scenes");

				SceneManager.AddScene();
				SceneManager.SetStartingScene(0);
			}

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
#ifdef BS_LOGGING_ON
		}

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
	}
#endif
}
