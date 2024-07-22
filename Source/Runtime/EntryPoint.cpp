#include "Bloodshot.h"

//initialization and deinitialization in components, entities, systems, allocators, scenes, etc...
//Add logs on instantiating, destroying entities, components, etc...

//TODO: separete EndPlay and EndSimulation, etc...

//To do �� ������� :
//+������� ������ �������� � �� �����������, ������� �������� ����������� � �� � Storages, ������� ������ �����, ��� ��
//+ ������� core systems ������� �������� ������ ������ ��� � ���� � ������ �� ��� ������,
//+������� ��� �� ��� �� �������������� � ECS � ��������� ����� ������� ����� ����� ������� ����� � ����� ������ ��������� � ���������� � ���������
//+ �������� � ������� ������� ������ � ����� ������� ������� ��������� ������ � ��������� ������
//+ ����������� ��������� � ����� ������ � ����� ����� ���� ������� ��� ����������� ����������(���� ��������)

int main(int argc, char** argv)
{
	Bloodshot::Logger::BeginSession();
	{
		//TODO: Create logger config and singleton this
		Bloodshot::ResourceManager::Config resourceManagerConfig = {};
		Bloodshot::MemoryManager::Config memoryManagerConfig = {.m_LinearMemorySize = 256_B, .m_ChunksToPreAllocate = 64, .m_BlocksPerChunk = 1024, .m_EntitiesStorageGrow = 1024, .m_ComponentsStorageGrow = 4096};
		Bloodshot::Window::Config windowConfig = {.m_TargetFPS = 0};
		Bloodshot::Renderer::Config rendererConfig = {};
		Bloodshot::ECS::Config ecsConfig = {};

		//Raylib::SetTraceLogLevel(Raylib::TraceLogLevel::LOG_NONE);
		Bloodshot::Logger::SetLogLevel(Bloodshot::LogLevel::LOG_TRACE);
		Bloodshot::Core::Create(resourceManagerConfig, memoryManagerConfig, rendererConfig, windowConfig, ecsConfig);
		Bloodshot::SceneManager::AddScene<Bloodshot::Test::TestScene>();
		Bloodshot::SceneManager::SetStartingScene<Bloodshot::Test::TestScene>();
		Bloodshot::Core::Run();
	}
	Bloodshot::Logger::EndSession();
}
