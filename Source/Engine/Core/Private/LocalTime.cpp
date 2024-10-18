#include "LocalTime.h"

#include <chrono>

namespace Bloodshot
{
	const char* ILocalTime::Now()
	{
		constexpr unsigned BufferSize = 128;
		static char Buffer[BufferSize];

		const time_t Now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		strftime(Buffer, BufferSize, "[%Y-%m-%d] [%X]", localtime(&Now));
		return Buffer;
	}
}
