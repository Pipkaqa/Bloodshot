#include "EngineContext.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Input.h"
#include "OpenGL/OpenGLHeader.h"
#include "OpenGL/OpenGLRenderer.h"
#include "OpenGL/OpenGLWindow.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "Systems/NetworkingSystem.h"
#include "Systems/RenderingSystem.h"

namespace Bloodshot::Private
{
	void IEngineContext::PreInit()
	{
#if defined(BS_DEBUG) && defined(BS_WINDOWS)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
		BS_PROFILE_RANGE("Core Init");
		BS_LOG(Debug, "Initializing the Core...");

		// BSTODO: Add Vulkan support
		//switch (Settings.RendererType)
		//{
		//	case ERendererType::OpenGL:
		//	{
		BS_LOG_IF(!(Window = TUniquePtr<IWindow>(new FOpenGLWindow)), Fatal, "Failed to create Window");
		BS_LOG_IF(!(Renderer = TUniquePtr<IRenderer>(new FOpenGLRenderer)), Fatal, "Failed to create Renderer");
		//
		//		break;
		//	}
		//	case ERendererType::Vulkan:
		//	{
		//		BS_LOG(Fatal, "Vulkan is not supported");
		//		break;
		//	}
		//}

		Window->Init();
		Renderer->Init();

		FEngineState::GetInstance().bRunning = true;
	}

	void IEngineContext::Dispose()
	{
		BS_PROFILE_RANGE("Core Dispose");
		BS_LOG(Debug, "Destroying the Core...");

		Window->Dispose();
		Renderer->Dispose();

		Private::FObjectCore::GetInstance().Dispose();
		FEngineState::GetInstance().bRunning = false;
	}

	void IEngineContext::BeginPlay()
	{
#ifdef BS_EXCEPTION_SAFETY_ON
		try
		{
#endif
			BS_PROFILE_RANGE("BeginPlay");
			BS_LOG(Debug, "BeginPlay");

			FSceneManager::GetInstance().BeginPlay();
#ifdef BS_EXCEPTION_SAFETY_ON
		}
		catch (const std::exception& Exception)
		{
			BS_LOG(Error, "Exception occurred in BeginPlay: {}", Exception.what());
		}
		catch (...)
		{
			BS_LOG(Error, "Undefined exception occurred in BeginPlay");
		}
#endif
	}

	void IEngineContext::EndPlay()
	{
#ifdef BS_EXCEPTION_SAFETY_ON
		try
		{
#endif
			BS_PROFILE_RANGE("EndPlay");
			BS_LOG(Debug, "EndPlay");

			FSceneManager::GetInstance().EndPlay();
#ifdef BS_EXCEPTION_SAFETY_ON
		}
		catch (const std::exception& Exception)
		{
			BS_LOG(Error, "Exception occurred in EndPlay: {}", Exception.what());
		}
		catch (...)
		{
			BS_LOG(Error, "Undefined exception occurred in EndPlay");
		}
#endif
	}

	void FEngineEditorContext::LoadConfig(Shared::FCmdParser& CmdParser)
	{
		//CmdParser.AddOption("Project");
		//CmdParser.Parse();
		//
		//if (!CmdParser.HasAllOptions())
		//{
		//	CmdParser.BuildErrorMessage();
		//	BS_LOG(Fatal, "{}", CmdParser.GetErrorMessage());
		//}
		//
		//const std::filesystem::path& ProjectPath = CmdParser.GetOptionValue(0);
	}

	void FEngineEditorContext::Init()
	{
		// BSTODO: Store into .bsproject file
		//GameDLLObserver.SetTimestampPath(CurrentProject.Directory.string() + "/Binaries/Timestamp");
	}

	void FEngineEditorContext::Run()
	{
#ifdef BS_WITH_AUTOMATION_TESTS
		FAutomationTestFramework::GetInstance().RunAllTests();
#endif
		while (!Window->ShouldClose() && !FApplication::GetInstance().bExitRequested)
		{
#ifdef BS_EXCEPTION_SAFETY_ON
			try
			{
#endif
				BS_PROFILE_RANGE("Main Loop");

				const float DeltaTime = IWindow::GetFrameTime();

				FrameTimeAccumulation += DeltaTime;
				++FrameCount;

				if (FrameTimeAccumulation >= 1000.f / (float)FPSUpdateRatePerSec)
				{
					AverageFrameTime = FrameTimeAccumulation / (float)FrameCount;
					FrameTimeAccumulation = 0.f;
					FrameCount = 0;
				}

				FEngineTime::GetInstance().Tick();
				Window->BeginFrame();

				static bool bSimulationStartedNow = false;

				bool& bSimulating = FEngineState::GetInstance().bSimulating;

				if (bSimulating)
				{
					if (!bSimulationStartedNow)
					{
						bSimulationStartedNow = true;
					}
				}

				if (bSimulationStartedNow)
				{
					BeginPlay();
					bSimulationStartedNow = false;
					bSimulating = true;
				}

#ifdef BS_NETWORKING_ON
				if (bSimulating) Networking::Private::FNetworkingSystem::Execute(DeltaTime);
#endif
				Window->PollEvents();
				if (bSimulating) FSceneManager::GetInstance().Tick(DeltaTime);
				Renderer->ClearBackground();
				FRenderingSystem::Execute(DeltaTime, Renderer.GetReference());

				//ImGui_ImplOpenGL3_NewFrame();
				//ImGui_ImplGlfw_NewFrame();
				//ImGui::NewFrame();
				//ImGui::Begin("Debug Info");
				//ImGui::Text(std::format("Frame time: {}", AverageFrameTime).c_str());
				//ImGui::Text(std::format("FPS: {}", (size_t)(1000.f / AverageFrameTime)).c_str());
				//ImGui::End();
				//ImGui::Render();
				//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				//
				//GLFWwindow* CurrentContext = glfwGetCurrentContext();
				//ImGui::UpdatePlatformWindows();
				//ImGui::RenderPlatformWindowsDefault();
				//glfwMakeContextCurrent(CurrentContext);

				Window->SwapBuffers();
				//OnDrawGizmos...
				//OnGUI...
				Window->EndFrame();

				if (bSimulationEndedNow)
				{
					EndPlay();
					bSimulating = false;
					bSimulationEndedNow = false;
				}
#ifdef BS_EXCEPTION_SAFETY_ON
			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred: {}", Exception.what());
			}
			catch (...)
			{
				BS_LOG(Error, "Undefined exception occurred");
			}
#endif
			//GameDLLObserver.Observe();
		}
	}

	void FEngineEditorContext::Exit()
	{

	}

	void FEngineGameContext::LoadConfig(Shared::FCmdParser& CmdParser)
	{

	}

	void FEngineGameContext::Init()
	{

	}

	void FEngineGameContext::Run()
	{

	}

	void FEngineGameContext::Exit()
	{

	}
}