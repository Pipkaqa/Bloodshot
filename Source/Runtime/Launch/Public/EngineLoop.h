#pragma once

#include "EngineContext.h"

namespace Bloodshot
{
	class FEngineLoop final
	{
	public:
		FEngineLoop();
		~FEngineLoop();

		void PreInit(Shared::FCmdParser& CmdParser);
		void Init();
		void Run();
		void Exit();
		void Dispose();

	private:
#ifdef BS_LOGGING_ON
		FLogger Logger;
#endif
#ifdef BS_PROFILING_ON
		FProfiler Profiler;
#endif
		IEngineContext* EngineContext = nullptr;
	};
}
