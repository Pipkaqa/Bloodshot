#include "Platform/Process.h"
#include "Containers/StringView.h"
#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"

namespace Bloodshot
{
	void IProcess::Create(FStringView Command)
	{
#ifdef BS_WINDOWS
		STARTUPINFO StartupInfo{sizeof(StartupInfo)};
		PROCESS_INFORMATION ProcessInfo{0};

		if (!CreateProcess(NULL, (LPSTR)Command.GetData(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
		{
			BS_LOG(Fatal, "IProcess::Create failed with error: {}", GetLastError());
			return;
		}

		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
#endif
	}
}
