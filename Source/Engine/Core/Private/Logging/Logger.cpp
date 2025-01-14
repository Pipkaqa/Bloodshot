#include "AssertionMacros.h"
#include "Casts.h"
#include "Logging/Logger.h"
#include "Logging/LoggingMacros.h"
#include "Memory/Memory.h"

namespace Bloodshot
{
	void FLogger::BeginSession(const ELogLevel LogLevelFlags,
		const EFileOpenMode OutputFileOpenMode,
		const bool bAlwaysWriteToFile)
	{
		bool& bStarted = bSessionStarted;

		BS_ASSERT(!bStarted, "Attempting to start already started Logging Session");

		CurrentLogLevelFlags = LogLevelFlags;

		bAlwaysWriteToOutputFile = bAlwaysWriteToFile;

		IFileIO::CreateIfNotExists("Logs");

		std::ofstream& Output = OutputFileStream;

		Output.open("Logs/Logger.txt", StaticCast<int>(OutputFileOpenMode));

		BS_ASSERT(Output.is_open(), "Failed to open Logger Output File");

		BS_LOG(Info, "Logging Session started");

		bStarted = true;
	}

	void FLogger::EndSession()
	{
		BS_ASSERT(bSessionStarted, "Attempting to end not started Logging Session");

		BS_LOG(Info, "Logging Session ended");

		OutputFileStream.close();
	}
}
