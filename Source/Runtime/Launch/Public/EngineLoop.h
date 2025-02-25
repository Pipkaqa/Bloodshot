#pragma once

#include "EngineContext.h"

namespace Bloodshot::Private::Launch
{
	class FEngineLoop final
	{
	public:
		FEngineLoop() {}
		~FEngineLoop() {}

		void PreInit(Shared::FCmdParser& CmdParser);
		void Init();
		void Run();
		void Exit();
		void Dispose();

	private:
		IEngineContext* EngineContext = nullptr;
	};
}
