#include "Platform/Process.h"
#include "Logging/LoggingMacros.h"

namespace Bloodshot
{
	void IProcess::Create(const std::string& Command)
	{
#ifdef BS_WINDOWS
		STARTUPINFO StartupInfo{sizeof(StartupInfo)};
		PROCESS_INFORMATION ProcessInfo{0};

		if (!CreateProcess(NULL, (LPSTR)Command.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
		{
			BS_LOG(Fatal, "CreateProcess() failed, error: {}", GetLastError());
			return;
		}

		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
#endif
	}
}
