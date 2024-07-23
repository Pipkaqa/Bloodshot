#include "Bloodshot.h"

//initialization and deinitialization in components, entities, systems, allocators, scenes, etc...
//Add logs on instantiating, destroying entities, components, etc...

//TODO: separete InternalEndPlay and EndSimulation, etc...

//To do на сегодня :
//+Сделать ентити менеджер и тд синглтонами, вынести хранение компонентов и тд в Storages, которые хранит сцена, так же
//+ сделать core systems которые менеджер систем хранит уже в себе и движок из сам создаёт,
//+вынести так же АПИ по взаимодействию в ECS в отдельный класс который будет брать текущую сцену и брать нужные сторэйджи и передавать в менеджеры
//+ Встроить в макросы логгера формат и чтобы макросы логгера принимали формат и аргументы внутрь
//+ оптимизируй аллокатор в конце концов и чтобы можно было удалять без инвалидации итераторов(если возможно)

int main(int argc, char** argv)
{
	Bloodshot::Logger::BeginSession();
	{
		//TODO: Create logger config and singleton this
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
}
