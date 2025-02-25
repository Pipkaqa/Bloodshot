#include "EngineLoop.h"

namespace Bloodshot::Private::Launch
{
	void FEngineLoop::PreInit(Shared::FCmdParser& CmdParser)
	{
		// BSTODO: Load config

		bool bEditor = true;

		if (bEditor)
		{
			EngineContext = new FEngineEditorContext;
		}
		else
		{
			EngineContext = new FEngineGameContext;
		}

		EngineContext->LoadConfig(CmdParser);
		EngineContext->PreInit();
	}

	void FEngineLoop::Init()
	{
		EngineContext->Init();
	}

	void FEngineLoop::Run()
	{
		EngineContext->Run();
	}

	void FEngineLoop::Exit()
	{
		EngineContext->Exit();
	}

	void FEngineLoop::Dispose()
	{
		EngineContext->Dispose();
		delete EngineContext;
	}
}
