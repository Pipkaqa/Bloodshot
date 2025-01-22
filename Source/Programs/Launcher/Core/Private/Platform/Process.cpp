#include "Platform/Process.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	void IProcess::Create(std::string_view Command)
	{
#ifdef BS_WINDOWS
		STARTUPINFO StartupInfo{sizeof(StartupInfo)};
		PROCESS_INFORMATION ProcessInfo{0};

		if (!CreateProcess(NULL, (LPSTR)Command.data(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
		{
#ifdef BS_DEBUG
			BS_DEBUG_BREAK();
#endif
			BS_TERMINATE();
		}
		else
		{
			CloseHandle(ProcessInfo.hProcess);
			CloseHandle(ProcessInfo.hThread);
		}
#endif
	}
}
