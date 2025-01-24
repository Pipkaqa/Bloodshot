#include "Editor.h"
#include "ImGuiHeader.h"

#include <OpenGL/OpenGLHeader.h>
#include <OpenGL/OpenGLRenderer.h>
#include <OpenGL/OpenGLWindow.h>

namespace Bloodshot::Editor::Private
{
	FEditor::FEditor(const std::filesystem::path& ProjectPath)
		: CurrentProject(ProjectPath)
	{
		// BSTODO: Store into .bsproject file
		GameDLLObserver.SetTimestampPath(CurrentProject.Directory.string() + "/Binaries/Timestamp");

		Init();
		RunLoop();
	}

	FEditor::~FEditor()
	{
		Dispose();
	}

	void FEditor::Init()
	{
		BS_LOG(Debug, "Creating the Core...");

		// BSTODO: Add Vulkan support
		switch (Settings.RendererType)
		{
			case ERendererType::OpenGL:
			{
				BS_LOG_IF(!(Window = TUniquePtr<IWindow>(new FOpenGLWindow())), Fatal, "Failed to create Window");
				BS_LOG_IF(!(Renderer = TUniquePtr<IRenderer>(new FOpenGLRenderer())), Fatal, "Failed to create Renderer");

				break;
			}
			case ERendererType::Vulkan:
			{
				BS_LOG(Fatal, "Vulkan is not supported");

				break;
			}
		}

		BS_LOG(Debug, "Initializing the Core...");
		{
			BS_PROFILE_RANGE("Initializing the Core");

			EngineState.Init();
			EngineTime.Init();
			ResourceManager.Init();
			Window->Init();
			Renderer->Init();
			EntityManager.Init();
			ComponentManager.Init();
			SystemManager.Init();
			SceneManager.Init();

			EngineState.bRunning = true;
		}

		ImGui::CreateContext();

		ImGui_ImplOpenGL3_Init("#version 460");
		ImGui_ImplGlfw_InitForOpenGL(Window.GetReference().As<FOpenGLWindow>()->Window, true);

		ImGui::StyleColorsDark();

		ImGuiIO& GuiIO = ImGui::GetIO();
		GuiIO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		GuiIO.Fonts->AddFontDefault();
	}

	void FEditor::Dispose()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

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
			EngineTime.Dispose();
			EngineState.Dispose();

			EngineState.bRunning = false;
		}
	}

	void FEditor::RunLoop()
	{
		const size_t FPSUpdateRatePerSec = 1;
		float FrameTimeAccumulation = 0.f;
		float AverageFrameTime = 0.f;
		size_t FrameCount = 0;

		while (!Window->ShouldClose() && !Application.bShouldClose)
		{
			try
			{
				BS_PROFILE_RANGE("Main Loop Tick");

				const float DeltaTime = IWindow::GetFrameTime();

				FrameTimeAccumulation += DeltaTime;
				++FrameCount;

				if (FrameTimeAccumulation >= 1000.f / (float)FPSUpdateRatePerSec)
				{
					AverageFrameTime = FrameTimeAccumulation / (float)FrameCount;
					FrameTimeAccumulation = 0.f;
					FrameCount = 0;
				}

				EngineTime.Tick();
				Window->BeginFrame();

				if (bSimulationStartedNow)
				{
					BeginPlay();
					bSimulationStartedNow = false;
					bSimulating = true;
				}

#ifdef BS_NETWORKING_ON
				if (bSimulating) NetworkingSystem.Execute(DeltaTime);
#endif
				Window->PollEvents();
				if (bSimulating) SceneManager.Tick(DeltaTime);
				Renderer->ClearBackground();
				RenderingSystem.Execute(DeltaTime, Renderer.GetReference());

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				ImGui::Begin("Debug Info");
				ImGui::Text(std::format("Frame time: {}", AverageFrameTime).c_str());
				ImGui::Text(std::format("FPS: {}", (size_t)(1000.f / AverageFrameTime)).c_str());
				ImGui::End();
				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				GLFWwindow* CurrentContext = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(CurrentContext);

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
			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred: {0}", Exception.what());
			}
			catch (...)
			{
				BS_LOG(Error, "Undefined Exception occurred");
			}

			GameDLLObserver.Observe();
		}
	}

	void FEditor::BeginPlay()
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

	void FEditor::EndPlay()
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

	void FEditor::FGameDLLObserver::SetTimestampPath(const std::filesystem::path& TimestampPath)
	{
		this->TimestampPath = TimestampPath.string();
	}

	void FEditor::FGameDLLObserver::Observe()
	{
		InputStream.open(TimestampPath);
		std::getline(InputStream, NewTimestamp);

		if (OldTimestamp != NewTimestamp)
		{
			OldTimestamp = NewTimestamp;

			// Reload game dll...
			BS_LOG(Info, "Reloading game dll...");
		}

		InputStream.close();
	}
}
