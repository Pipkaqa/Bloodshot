#include "Editor.h"
#include "ImGuiHeader.h"

#include <AssertionMacros.h>
#include <Logging/LoggingMacros.h>
#include <OpenGL/OpenGLHeader.h>
#include <OpenGL/OpenGLRenderer.h>
#include <OpenGL/OpenGLWindow.h>
#include <Profiling/ProfilingMacros.h>

namespace Bloodshot::Editor::Private
{
	FEditor::FEditor()
	{
		Init();
		SetTestScene();
		BeginPlay();
		Run();
	}

	FEditor::~FEditor()
	{
		EndPlay();
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
		ImGui_ImplGlfw_InitForOpenGL(((FOpenGLWindow*)Window.get())->Window, true);

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

	void FEditor::Run()
	{
		float FrameTimeAccumulation = 0.f;

		while (!Window->ShouldClose())
		{
			try
			{
				BS_PROFILE_RANGE("Main Loop Tick");

				const float DeltaTime = IWindow::GetFrameTime();

				EngineTime.Tick();
				Window->BeginFrame();
#ifdef BS_NETWORKING_ON
				NetworkingSystem.Execute(DeltaTime);
#endif
				Window->PollEvents();
				SceneManager.Tick(DeltaTime);
				Renderer->ClearBackground();
				RenderingSystem.Execute(DeltaTime, Renderer);

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();
				ImGui::Begin("Debug Info");
				ImGui::Text(std::format("Frame time: {0}", DeltaTime).c_str());
				ImGui::Text(std::format("FPS: {0}", Window->GetFramesPerSecond()).c_str());
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

	void FEditor::SetTestScene()
	{
		BS_LOG(Debug, "Adding Scenes...");
		{
			BS_PROFILE_RANGE("Adding Scenes");

			SceneManager.AddScene();
			SceneManager.SetStartingScene(0);
		}
	}
}
