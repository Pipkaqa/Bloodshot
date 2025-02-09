#include "Logging/Logger.h"
#include "Logging/LoggingMacros.h"
#include "Misc/AssertionMacros.h"

namespace Bloodshot
{
	FLogger& FLogger::GetInstance()
	{
		static FLogger Instance;
		return Instance;
	}

	void FLogger::BeginSession(const ELogLevel InLogLevelFlags,
		const EFileOpenMode InOutputFileOpenMode,
		const bool bInAlwaysWriteToFile)
	{
		bool& bStarted = bSessionStarted;

		BS_ASSERT(!bStarted, "Attempting to start already started Logging Session");

		CurrentLogLevelFlags = InLogLevelFlags;

		bAlwaysWriteToFile = bInAlwaysWriteToFile;

		IFileIO::CreateIfNotExists("Logs");

		std::ofstream& Output = OutputStream;

		Output.open("Logs/Logger.txt", (int)InOutputFileOpenMode);

		BS_ASSERT(Output.is_open(), "Failed to open Logger Output File");
		BS_LOG(Info, "Logging Session started");

		bStarted = true;
	}

	void FLogger::EndSession()
	{
		BS_ASSERT(bSessionStarted, "Attempting to end not started Logging Session");
		BS_LOG(Info, "Logging Session ended");

		OutputStream.close();
	}
}
