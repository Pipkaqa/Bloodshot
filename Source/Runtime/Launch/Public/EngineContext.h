#pragma once

#include "Core.h"

#include "CmdParser.h"
#include "ComponentManager.h"
#include "EntityManager.h"
#include "Input.h"
#include "Project.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "SystemManager.h"
#include "Systems/NetworkingSystem.h"
#include "Systems/RenderingSystem.h"
#include "Window.h"

namespace Bloodshot
{
	class IEngineContext : public INonCopyable
	{
	public:
		IEngineContext() = default;
		virtual ~IEngineContext() {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) = 0;
		virtual void Init() = 0;
		virtual void Run() = 0;
		virtual void Exit() = 0;

		void PreInit();
		void Dispose();

	protected:
		FMemory Memory;
		FEngineState EngineState;
		FEngineTime EngineTime;
		TUniquePtr<IWindow> Window = nullptr;
		TUniquePtr<IRenderer> Renderer = nullptr;
		FEntityManager EntityManager;
		FComponentManager ComponentManager;
		FSystemManager SystemManager;
		FSceneManager SceneManager;
		FApplication Application;
		Bloodshot::Private::FRenderingSystem RenderingSystem;
#ifdef BS_NETWORKING_ON
		Bloodshot::Networking::Private::FNetworkingSystem NetworkingSystem;
#endif
	};

	class FEngineEditorContext : public IEngineContext
	{
	public:
		FEngineEditorContext() = default;

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) override;
		virtual void Init() override;
		virtual void Run() override;
		virtual void Exit() override;

	private:
		// FEditor Editor

		const size_t FPSUpdateRatePerSec = 1;
		float FrameTimeAccumulation = 0.f;
		float AverageFrameTime = 0.f;
		size_t FrameCount = 0;

		//class FGameDLLObserver
		//{
		//public:
		//	void SetTimestampPath(const std::filesystem::path& TimestampPath);
		//	void Observe();
		//
		//private:
		//	std::string TimestampPath;
		//	std::string OldTimestamp;
		//	std::string NewTimestamp;
		//	std::ifstream InputStream;
		//};
		//
		//FSettings Settings;
		//FGameDLLObserver GameDLLObserver;

		FProject CurrentProject;

		bool bSimulationStartedNow = false;
		bool bSimulating = false;
		bool bSimulationEndedNow = false;

		void BeginPlay();
		void EndPlay();
	};

	class FEngineGameContext : public IEngineContext
	{
	public:
		FEngineGameContext() = default;

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) override;
		virtual void Init() override;
		virtual void Run() override;
		virtual void Exit() override;

	private:
		void BeginPlay();
		void EndPlay();
	};
}

//void FEditor::FGameDLLObserver::SetTimestampPath(const std::filesystem::path& TimestampPath)
//{
//	this->TimestampPath = TimestampPath.string();
//}
//
//void FEditor::FGameDLLObserver::Observe()
//{
//	InputStream.open(TimestampPath);
//	std::getline(InputStream, NewTimestamp);
//
//	if (OldTimestamp != NewTimestamp)
//	{
//		OldTimestamp = NewTimestamp;
//
//		// Reload game dll...
//		BS_LOG(Info, "Reloading game dll...");
//	}
//
//	InputStream.close();
//}