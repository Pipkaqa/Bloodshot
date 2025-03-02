#pragma once

#include "Core.h"

#include "CmdParser.h"
#include "Project.h"
#include "Renderer.h"
#include "Window.h"

namespace Bloodshot::Private::Launch
{
	class IEngineContext
	{
	public:
		IEngineContext() = default;
		IEngineContext(const IEngineContext&) = delete;
		IEngineContext& operator=(const IEngineContext&) = delete;
		virtual ~IEngineContext() {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) = 0;
		virtual void Init() = 0;
		virtual void Run() = 0;
		virtual void Exit() = 0;

		void PreInit();
		void Dispose();

		void BeginPlay();
		void EndPlay();

	protected:
		TUniquePtr<IWindow> Window = nullptr;
		TUniquePtr<IRenderer> Renderer = nullptr;
	};

	class FEngineEditorContext : public IEngineContext
	{
	public:
		FEngineEditorContext() = default;
		virtual ~FEngineEditorContext() override {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) override;
		virtual void Init() override;
		virtual void Run() override;
		virtual void Exit() override;

	private:
		const size_t FPSUpdateRatePerSec = 1;
		float FrameTimeAccumulation = 0.f;
		float AverageFrameTime = 0.f;
		size_t FrameCount = 0;

		Shared::FProject CurrentProject;

		bool bSimulationStartedNow = false;
		bool bSimulating = false;
		bool bSimulationEndedNow = false;
	};

	class FEngineGameContext : public IEngineContext
	{
	public:
		FEngineGameContext() = default;
		virtual ~FEngineGameContext() override {}

		virtual void LoadConfig(Shared::FCmdParser& CmdParser) override;
		virtual void Init() override;
		virtual void Run() override;
		virtual void Exit() override;
	};
}

// FEditor Editor

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
