// BSTODO:
//		Event system, what handles: scene reloading, key presses, any input, window resizing, application exit, etc...
//		Add new allocators, minimize new calls, maximize cache locality
//		Inject entry point in engine

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
#include <TestFramework.h>
#include <Window.h>

#include <exception>
#include <filesystem>

int main(int Argc, char** Argv)
{
	using namespace Bloodshot;
	//using namespace Bloodshot::Private;

	FMemory::BeginSession();
	BS_CHECK(FMemory::IsSessionStarted(), "Memory Logging Session not started!");

#ifdef BS_LOGGING_ON
	{
		ILogger::BeginSession(~(ELogLevel::Trace), EFileOpenMode::Truncate);
		BS_CHECK(ILogger::IsSessionStarted(), "Logging Session not started!");
		{
#endif
#ifdef BS_PROFILING_ON
			TUniquePtr<SProfiler> Profiler = MakeUnique<SProfiler>();
			Profiler->BeginSession();
			BS_CHECK(Profiler->IsSessionStarted(), "Profiling Session not started!");
#endif
			BS_LOG(Debug, "Creating the Core...");

			TUniquePtr<FEngineState> EngineState = MakeUnique<FEngineState>();
			TUniquePtr<FEngineTime> EngineTime = MakeUnique<FEngineTime>();
			TUniquePtr<FResourceManager> ResourceManager = MakeUnique<FResourceManager>();

			TUniquePtr<IRenderer> Renderer = nullptr;
			TUniquePtr<IWindow> Window = nullptr;

			// BSTODO: Temporary hard-coded
			ERendererType RendererType = ERendererType::OpenGL;

			// BSTODO: Add Vulkan support
			switch (RendererType)
			{
				case ERendererType::OpenGL:
				{
					BS_LOG_IF(!(Renderer = MakeUnique<FOpenGLRenderer>()), Fatal, "Failed to create Renderer");
					BS_LOG_IF(!(Window = MakeUnique<FOpenGLWindow>()), Fatal, "Failed to create Window");

					break;
				}
				case ERendererType::Vulkan:
				{
					BS_LOG(Fatal, "Vulkan is not supported");
				}
			}

			TUniquePtr<FEntityManager> EntityManager = MakeUnique<FEntityManager>();
			TUniquePtr<FComponentManager> ComponentManager = MakeUnique<FComponentManager>();
			TUniquePtr<FSystemManager> SystemManager = MakeUnique<FSystemManager>();
			TUniquePtr<FSceneManager> SceneManager = MakeUnique<FSceneManager>();
			TUniquePtr<FApplication> Application = MakeUnique<FApplication>(Window);

			// BSTODO: Temporary hard-coded
			FMemory::ChunksToPreAllocateByFixedAllocators = 4;
			FMemory::BlocksPerChunkInFixedAllocators = 1024;

			// BSTODO: Temporary hard-coded
			IECS::EntityStorageGrow = 1024;
			IECS::ComponentStorageGrow = 1024;

			BS_LOG(Debug, "Initializing the Core...");
			{
				BS_PROFILE_RANGE("Initializing the Core");

				EngineState->Init();
				EngineTime->Init();
				ResourceManager->Init();
				Window->Init();
				Renderer->Init();
				EntityManager->Init();
				ComponentManager->Init();
				SystemManager->Init();
				SceneManager->Init();

				const std::string& CurrentWorkingDirectory = std::filesystem::current_path().string();

				BS_LOG(Info, "Working directory: {0}", CurrentWorkingDirectory);
				BS_LOG(Info, "Log directory: {0}\\Log", CurrentWorkingDirectory);
				BS_LOG(Info, "Resources directory: {0}\\{1}", CurrentWorkingDirectory, FResourceManager::ResourcesPath);

				EngineState->bRunning = true;
			}

			BS_LOG(Debug, "Adding Scenes...");
			{
				BS_PROFILE_RANGE("Adding Scenes");

				FSceneManager::AddScene<Test::FTestScene>();
				FSceneManager::SetStartingScene<Test::FTestScene>();
			}

			try
			{
				BS_LOG(Debug, "Starting simulation...");
				{
					BS_PROFILE_RANGE("Starting Simulation");

					SceneManager->BeginSimulation();
					EngineState->bSimulating = true;
					SceneManager->InternalBeginPlay();
				}
			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred in Startup-Block: {0}", Exception.what());
			}

			while (!Window->ShouldClose())
			{
				try
				{
					BS_PROFILE_RANGE("Main Loop Tick");

					EngineTime->Tick();

					Window->BeginFrame();
					SceneManager->InternalUpdate(IWindow::GetFrameTime(), Renderer, Window);
					Window->EndFrame();

					//static float FrameTimeAccumulation = 0.f;
					//
					//FrameTimeAccumulation += Window->GetFrameTime();
					//
					//if (FrameTimeAccumulation >= 1000.f)
					//{
					//	BS_LOG(Info, "Frame Time: {0} ms", Window->GetFrameTime());
					//	BS_LOG(Info, "FPS: {0} ms", Window->GetFramesPerSecond());
					//
					//	FrameTimeAccumulation = 0.f;
					//}
				}
				catch (const std::exception& Exception)
				{
					BS_LOG(Error, "Exception occurred: {0}", Exception.what());
				}
			}

			try
			{
				BS_LOG(Debug, "Ending simulation...");
				{
					BS_PROFILE_RANGE("Shutdown");

					SceneManager->InternalEndPlay();
					SceneManager->EndSimulation();

					EngineState->bSimulating = false;
				}
			}
			catch (const std::exception& Exception)
			{
				BS_LOG(Error, "Exception occurred in Shutdown-Block: {0}", Exception.what());
			}

			BS_LOG(Debug, "Destroying the Core...");
			{
				BS_PROFILE_RANGE("Dispose");

				SceneManager->Dispose();
				SystemManager->Dispose();
				ComponentManager->Dispose();
				EntityManager->Dispose();
				Renderer->Dispose();
				Window->Dispose();
				ResourceManager->Dispose();
				EngineTime->Dispose();
				EngineState->Dispose();

				EngineState->bRunning = false;
			}

#ifdef BS_PROFILING_ON
			Profiler->EndSession();
#endif
#ifdef BS_LOGGING_ON
		}

		auto ConvertByToKib = [](const size_t Size) { return Size >> 10; };
		auto ConvertByToMib = [](const size_t Size) { return Size >> 20; };
		auto ConvertByToGib = [](const size_t Size) { return Size >> 30; };

		const FMemory::FDefaultAllocatorsMemoryInfo SAMI = FMemory::StaticAllocationsMemoryInfo;

		BS_LOG(Info, "Statically Allocated:              {0}.B, {1}.MB, {2} Blocks",
			SAMI.AllocatedSizeByNew,
			ConvertByToMib(SAMI.AllocatedSizeByNew),
			SAMI.NewCallCount);

		const FMemory::FDefaultAllocatorsMemoryInfo DAMI = FMemory::DefaultAllocatorsMemoryInfo;
		const FMemory::FEngineAllocatorsMemoryInfo EAMI = FMemory::EngineAllocatorsMemoryInfo;

		BS_LOG(Info, "Allocated by Default Allocators:   {0}.B, {1}.MB, {2} Blocks",
			DAMI.AllocatedSizeByNew,
			ConvertByToMib(DAMI.AllocatedSizeByNew),
			DAMI.NewCallCount);

		BS_LOG(Info, "Deallocated by Default Allocators: {0}.B, {1}.MB, {2} Blocks",
			DAMI.DeallocatedSizeByDelete,
			ConvertByToMib(DAMI.DeallocatedSizeByDelete),
			DAMI.DeleteCallCount);

		BS_LOG(Info, "Allocated By Engine Allocators:    {0}.B, {1}.MB, {2} Blocks",
			EAMI.AllocatedSize,
			ConvertByToMib(EAMI.AllocatedSize),
			EAMI.AllocatedBlockCount);

		BS_LOG(Info, "Deallocated By Engine Allocators:  {0}.B, {1}.MB, {2} Blocks",
			EAMI.DeallocatedSize,
			ConvertByToMib(EAMI.DeallocatedSize),
			EAMI.DeallocatedBlockCount);

		BS_LOG(Info, "Total Allocated:                   {0}.B, {1}.MB, {2} Blocks",
			DAMI.AllocatedSizeByNew + EAMI.AllocatedSize,
			ConvertByToMib(DAMI.AllocatedSizeByNew + EAMI.AllocatedSize),
			DAMI.NewCallCount + EAMI.AllocatedBlockCount);

		BS_LOG(Info, "Total Deallocated:                 {0}.B, {1}.MB, {2} Blocks",
			DAMI.DeallocatedSizeByDelete + EAMI.DeallocatedSize,
			ConvertByToMib(DAMI.DeallocatedSizeByDelete + EAMI.DeallocatedSize),
			DAMI.DeleteCallCount + EAMI.DeallocatedBlockCount);

		FMemory::IsMemoryLeak() ? BS_LOG(Warning, "Memory leak detected") : BS_LOG(Info, "Memory leak not detected");

		ILogger::EndSession();
	}
#endif

	FMemory::EndSession();

	//Template code

	/*
	GLFWwindow* window;
	if (!glfwInit())
	{
		BS_LOG(Fatal, "Failed to init glfw");
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	struct WindowData
	{
		glm::uvec2 m_Resolution = {640, 480};
	};

	WindowData windowData = {{700, 550}};

	window = glfwCreateWindow(700, 550, "OpenGL", nullptr, nullptr);

	if (!window)
	{
		BS_LOG(Fatal, "Failed to create OpenGL window");
		glfwTerminate();
	}

	glfwSetWindowUserPointer(window, &windowData);

	auto onWindowResize = [](GLFWwindow* window, int x, int y)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.m_Resolution = {x, y};
			glViewport(0, 0, x, y);
		};

	glfwSetWindowSizeCallback(window, onWindowResize);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!gladLoadGL())
	{
		BS_LOG(Fatal, "Failed to load glad");
	}

	BS_LOG(Info, "OpenGL {0}.{1} has been loaded!", GLVersion.major, GLVersion.minor);
	BS_LOG(Info, "Current working directory: {0}", std::filesystem::current_path().string());

	glClearColor(0, 0, 0, 1);

	std::ifstream ifstream;
	ifstream.open("VertexShader.glsl");

	if (!ifstream.is_open())
	{
		BS_LOG(Fatal, "Cannot read vertex shader source from VertexShader.glsl!");
	}

	std::stringstream vertexShaderStream;
	vertexShaderStream << ifstream.rdbuf();
	std::string vertexShaderSource = vertexShaderStream.str();

	ifstream.close();
	ifstream.open("FragmentShader.glsl");

	if (!ifstream.is_open())
	{
		BS_LOG(Fatal, "Cannot read fragment shader source from FragmentShader.glsl!");
	}

	std::stringstream fragmentShaderStream;
	fragmentShaderStream << ifstream.rdbuf();
	std::string fragmentShaderSource = fragmentShaderStream.str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertexShaderSourceRaw = vertexShaderSource.c_str();
	const char* fragmentShaderSourceRaw = fragmentShaderSource.c_str();

	glShaderSource(vertexShader, 1, &vertexShaderSourceRaw, nullptr);
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceRaw, nullptr);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	GLint result;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		GLint length;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);

		GLchar* log = new GLchar[length + 1];
		glGetShaderInfoLog(vertexShader, length, &length, log);

		BS_LOG(Fatal, "Vertex shader compile error: {0}", log);
	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

	if (!result)
	{
		GLint length;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);

		GLchar* log = new GLchar[length + 1];
		glGetShaderInfoLog(fragmentShader, length, &length, log);

		BS_LOG(Fatal, "Fragment shader compile error: {0}", log);
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);

	if (!result)
	{
		GLint length;
		glGetProgramiv(vertexShader, GL_INFO_LOG_LENGTH, &length);

		GLchar* log = new GLchar[length + 1];
		glGetProgramInfoLog(vertexShader, length, &length, log);

		BS_LOG(Fatal, "Shader program link error: {0}", log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	GLfloat points[12] =
	{
		-1.f, -1.f,
		-1.f,  1.f,
		 1.f,  1.f,
		-1.f, -1.f,
		 1.f,  1.f,
		 1.f, -1.f,
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	auto start = std::chrono::high_resolution_clock::now();
	float globalTime = 0;
	float frameTime = 0;
	float frameTimeAccumulation = 0;
	size_t frameCount = 0;

	while (!glfwWindowShouldClose(window))
	{
		auto frameBegin = std::chrono::high_resolution_clock::now();
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(shaderProgram);
			glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), 700, 550);
			glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), globalTime);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		auto frameEnd = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> frameDuration = frameEnd - frameBegin;
		frameTime = frameDuration.count() * 1000;
		frameTimeAccumulation += frameDuration.count();
		frameCount++;

		if (frameTimeAccumulation >= 1.f)
		{
			BS_LOG(Info, "GetGlobalTime time: {0} s", globalTime);
			BS_LOG(Info, "Frame time: {0} ms", frameTime);
			BS_LOG(Info, "FPS: {0}", frameCount);
			BS_LOG(Info, "Resolution: {0}:{1}", windowData.m_Resolution.x, windowData.m_Resolution.y);

			frameTimeAccumulation = 0.f;
			frameCount = 0;
		}

		auto current = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = current - start;
		globalTime = duration.count();
	}

	glfwTerminate();
	*/
}
