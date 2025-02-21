#include "Logging/Logger.h"
#include "Logging/LoggingMacros.h"
#include "Misc/AssertionMacros.h"

#include <filesystem>

namespace Bloodshot
{
	FLogger& FLogger::GetInstance()
	{
		static FLogger Instance;
		return Instance;
	}

	void FLogger::BeginSession(const ELogLevel InLogLevelFlags,
		const int InOutputStreamOpenMode,
		const bool bInAlwaysWriteToFile)
	{
		bool& bStarted = bSessionStarted;
		BS_CHECK(!bStarted);

		CurrentLogLevelFlags = InLogLevelFlags;
		bAlwaysWriteToFile = bInAlwaysWriteToFile;

		if (!std::filesystem::exists("Logs"))
		{
			std::filesystem::create_directory("Logs");
		}

		std::ofstream& Output = OutputStream;
		Output.open("Logs/Logger.txt", InOutputStreamOpenMode);

		BS_CHECK(Output.is_open());
		BS_LOG(Info, "Logging Session started");

		bStarted = true;
	}

	void FLogger::EndSession()
	{
		BS_CHECK(bSessionStarted);
		BS_LOG(Info, "Logging Session ended");
		OutputStream.close();
	}
}
