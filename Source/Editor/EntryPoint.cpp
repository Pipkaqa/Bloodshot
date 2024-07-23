#include "Bloodshot.h"

#include <iostream>

int main(int argc, char** argv)
{
	Bloodshot::Logger::BeginSession();
	{
		Bloodshot::ResourceManager::Config resourceManagerConfig = {};
		Bloodshot::MemoryManager::Config memoryManagerConfig =
		{
			.m_LinearMemorySize = 256_B,
			.m_ChunksToPreAllocate = 64,
			.m_BlocksPerChunk = 1024,
			.m_EntitiesStorageGrow = 1024,
			.m_ComponentsStorageGrow = 4096
		};
		Bloodshot::Renderer::Config rendererConfig = {};
		Bloodshot::Window::Config windowConfig = {.m_TargetFPS = 0};
		Bloodshot::ECS::Config ecsConfig = {};

		Bloodshot::Logger::SetLogLevel(Bloodshot::LogLevel::LOG_TRACE);
		Bloodshot::Core::Create(resourceManagerConfig, memoryManagerConfig, rendererConfig, windowConfig, ecsConfig);
		Bloodshot::SceneManager::AddScene<Bloodshot::Test::TestScene>();
		Bloodshot::SceneManager::SetStartingScene<Bloodshot::Test::TestScene>();
		Bloodshot::Core::Run();
	}
	Bloodshot::Logger::EndSession();
	
	//GLFWwindow* window;
	//if (!glfwInit())
	//{
	//	return -1;
	//}
	//
	//window = glfwCreateWindow(700, 550, "OpenGL", nullptr, nullptr);
	//
	//if (!window)
	//{
	//	glfwTerminate();
	//	return -1;
	//}
	//
	//glfwMakeContextCurrent(window);
	//
	//if (!gladLoadGL())
	//{
	//	std::cout << "Can't load glad!\n";
	//	return -1;
	//}
	//
	//std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << " has been loaded\n";
	//
	//glClearColor(0, 1, 1, 1);
	//
	//while (!glfwWindowShouldClose(window))
	//{
	//	glClear(GL_COLOR_BUFFER_BIT);
	//
	//	glfwSwapBuffers(window);
	//
	//	glfwPollEvents();
	//}
	//
	//glfwTerminate();
}
