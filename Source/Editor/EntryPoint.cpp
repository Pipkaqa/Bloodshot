// BSTODO:
//		Exception catcher around the begin, game loop and end
//		Engine should not crash in most situations
//		Event system, what handles: scene reloading, key presses, any input, window resizing, application exit, etc...
//		Solve the memory leak problem
//		Add new allocators, minimize new calls, maximize cache locality

// IMPORTANT:
//		Rewrite the engine core, that allows do own application life pipeline in entry point. 
//		____At the moment the engine core is hard-linked inside, what hard to support.
//		I think:
//				Need to transform core to command line :D, and unlink all modules from it, 
//				____instead of better to link all submodules in entry point
//				And inject entry point in engine

#include <Bloodshot.h>

int main(int argc, char** argv)
{
	{
#ifdef BS_LOGGING_ON
		Bloodshot::Logger::Config loggerConfig = {.m_LogLevel = Bloodshot::LogLevel::LOG_TRACE};
		Bloodshot::Logger::BeginSession(loggerConfig);
#endif
		{
#ifdef BS_PROFILING_ON
			auto profiler = Bloodshot::Profiler::Create();
			Bloodshot::Profiler::BeginSession();
#endif
			Bloodshot::ResourceManager::Config resourceManagerConfig = {};
			Bloodshot::MemoryManager::Config memoryManagerConfig =
			{
				.m_LinearMemorySize = 4_KB,
				.m_ChunksToPreAllocate = 4,
				.m_BlocksPerChunk = 1024
			};
			Bloodshot::Renderer::Config rendererConfig = {};
			Bloodshot::Window::Config windowConfig = {};
			Bloodshot::ECS::Config ecsConfig =
			{
				.m_EntitiesStorageGrow = 64,
				.m_ComponentsStorageGrow = 192
			};

			auto core = Bloodshot::Core::Create(resourceManagerConfig, memoryManagerConfig, rendererConfig, windowConfig, ecsConfig);
			Bloodshot::SceneManager::AddScene<Bloodshot::Test::TestScene>();
			Bloodshot::SceneManager::SetStartingScene<Bloodshot::Test::TestScene>();
			Bloodshot::Core::Run();
			delete core;

#ifdef BS_PROFILING_ON
			Bloodshot::Profiler::EndSession();
			delete profiler;
#endif
		}

		Bloodshot::PrintMemoryStatus();

		Bloodshot::MemoryManager::MemoryLeak()
			? printf("\033[93m[WARNING]: Memory leak detected!\033[0m\n")
			: printf("\033[94m[INFO]: \033[0mMemory leak not detected\n");

#ifdef BS_LOGGING_ON
		Bloodshot::Logger::EndSession();
#endif
	}

	//Bloodshot::Logger::Log(memoryLeak ? Bloodshot::LogLevel::LOG_WARNING : Bloodshot::LogLevel::LOG_INFO, "Memory leak {0}", memoryLeak ? "detected" : "not detected");

	//Template code

	//GLFWwindow* window;
	//if (!glfwInit())
	//{
	//	BS_FATAL("Failed to init glfw!");
	//	TERMINATE;
	//}

	////glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	////glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	////glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//struct WindowData
	//{
	//	glm::uvec2 m_Resolution = {640, 480};
	//};

	//WindowData windowData = {{700, 550}};

	//window = glfwCreateWindow(700, 550, "OpenGL", nullptr, nullptr);

	//if (!window)
	//{
	//	BS_FATAL("Failed to create OpenGL window!");
	//	glfwTerminate();
	//	TERMINATE;
	//}

	//glfwSetWindowUserPointer(window, &windowData);

	//auto onWindowResize = [](GLFWwindow* window, int x, int y)
	//	{
	//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
	//		data.m_Resolution = {x, y};
	//		glViewport(0, 0, x, y);
	//	};

	//glfwSetWindowSizeCallback(window, onWindowResize);

	//glfwMakeContextCurrent(window);
	//glfwSwapInterval(0);

	//if (!gladLoadGL())
	//{
	//	BS_FATAL("Failed to load glad!");
	//	TERMINATE;
	//}

	//BS_INFO("OpenGL {0}.{1} has been loaded!", GLVersion.major, GLVersion.minor);
	//BS_INFO("Current working directory: {0}", std::filesystem::current_path().string());

	//glClearColor(0, 0, 0, 1);

	//std::ifstream ifstream;
	//ifstream.open("VertexShader.glsl");

	//if (!ifstream.is_open())
	//{
	//	BS_FATAL("Cannot read vertex shader source from VertexShader.glsl!");
	//	TERMINATE;
	//}

	//std::stringstream vertexShaderStream;
	//vertexShaderStream << ifstream.rdbuf();
	//std::string vertexShaderSource = vertexShaderStream.str();

	//ifstream.close();
	//ifstream.open("FragmentShader.glsl");

	//if (!ifstream.is_open())
	//{
	//	BS_FATAL("Cannot read fragment shader source from FragmentShader.glsl!");
	//	TERMINATE;
	//}

	//std::stringstream fragmentShaderStream;
	//fragmentShaderStream << ifstream.rdbuf();
	//std::string fragmentShaderSource = fragmentShaderStream.str();

	//GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//const char* vertexShaderSourceRaw = vertexShaderSource.c_str();
	//const char* fragmentShaderSourceRaw = fragmentShaderSource.c_str();

	//glShaderSource(vertexShader, 1, &vertexShaderSourceRaw, nullptr);
	//glShaderSource(fragmentShader, 1, &fragmentShaderSourceRaw, nullptr);

	//glCompileShader(vertexShader);
	//glCompileShader(fragmentShader);

	//GLint result;
	//glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);

	//if (!result)
	//{
	//	GLint length;
	//	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &length);

	//	GLchar* log = new GLchar[length + 1];
	//	glGetShaderInfoLog(vertexShader, length, &length, log);

	//	BS_FATAL("Vertex shader compile error: {0}", log);
	//	delete[] log;

	//	TERMINATE;
	//}

	//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);

	//if (!result)
	//{
	//	GLint length;
	//	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &length);

	//	GLchar* log = new GLchar[length + 1];
	//	glGetShaderInfoLog(fragmentShader, length, &length, log);

	//	BS_FATAL("Fragment shader compile error: {0}", log);
	//	delete[] log;

	//	TERMINATE;
	//}

	//GLuint shaderProgram = glCreateProgram();
	//glAttachShader(shaderProgram, vertexShader);
	//glAttachShader(shaderProgram, fragmentShader);
	//glLinkProgram(shaderProgram);

	//glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);

	//if (!result)
	//{
	//	GLint length;
	//	glGetProgramiv(vertexShader, GL_INFO_LOG_LENGTH, &length);

	//	GLchar* log = new GLchar[length + 1];
	//	glGetProgramInfoLog(vertexShader, length, &length, log);

	//	BS_FATAL("Shader program link error: {0}", log);
	//	delete[] log;

	//	TERMINATE;
	//}

	//glDeleteShader(vertexShader);
	//glDeleteShader(fragmentShader);

	//glUseProgram(shaderProgram);

	//GLfloat points[12] =
	//{
	//	-1.f, -1.f,
	//	-1.f,  1.f,
	//	 1.f,  1.f,
	//	-1.f, -1.f,
	//	 1.f,  1.f,
	//	 1.f, -1.f,
	//};

	//GLuint vbo;
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//auto start = std::chrono::high_resolution_clock::now();
	//float globalTime = 0;
	//float frameTime = 0;
	//float frameTimeAccumulation = 0;
	//size_t frameCount = 0;

	//while (!glfwWindowShouldClose(window))
	//{
	//	auto frameBegin = std::chrono::high_resolution_clock::now();
	//	{
	//		glClear(GL_COLOR_BUFFER_BIT);

	//		glUseProgram(shaderProgram);
	//		glUniform2f(glGetUniformLocation(shaderProgram, "iResolution"), 700, 550);
	//		glUniform1f(glGetUniformLocation(shaderProgram, "iTime"), globalTime);

	//		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//		glDrawArrays(GL_TRIANGLES, 0, 6);

	//		glfwSwapBuffers(window);
	//		glfwPollEvents();
	//	}
	//	auto frameEnd = std::chrono::high_resolution_clock::now();

	//	std::chrono::duration<float> frameDuration = frameEnd - frameBegin;
	//	frameTime = frameDuration.count() * 1000;
	//	frameTimeAccumulation += frameDuration.count();
	//	frameCount++;

	//	if (frameTimeAccumulation >= 1.f)
	//	{
	//		BS_INFO("Global time: {0} s", globalTime);
	//		BS_INFO("Frame time: {0} ms", frameTime);
	//		BS_INFO("FPS: {0}", frameCount);
	//		BS_INFO("Resolution: {0}:{1}", windowData.m_Resolution.x, windowData.m_Resolution.y);

	//		frameTimeAccumulation = 0.f;
	//		frameCount = 0;
	//	}

	//	auto current = std::chrono::high_resolution_clock::now();
	//	std::chrono::duration<float> duration = current - start;
	//	globalTime = duration.count();
	//}

	//glfwTerminate();
}
