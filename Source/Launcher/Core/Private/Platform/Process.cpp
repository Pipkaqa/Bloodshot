#include "Platform/Process.h"

namespace Bloodshot
{
	void IProcess::Create(const std::string& Command)
	{
#ifdef BS_WINDOWS
		STARTUPINFO StartupInfo{sizeof(StartupInfo)};
		PROCESS_INFORMATION ProcessInfo{0};

		if (!CreateProcess(NULL, (LPSTR)Command.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
		{
#ifdef BS_DEBUG_ON
			BS_DEBUG_BREAK;
#endif
			BS_TERMINATE;
		}
		else
		{
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
#endif
	}
}
