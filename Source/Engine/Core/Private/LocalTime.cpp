#include "LocalTime.h"

#include <chrono>

namespace Bloodshot
{
	const char* ILocalTime::Now()
	{
		// BSTODO: Protect from overflow without using new
		constexpr unsigned BufferSize = 128U;
		static char Buffer[BufferSize];

		const time_t Now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		strftime(Buffer, BufferSize, "[%Y-%m-%d] [%X]", localtime(&Now));
		return Buffer;
	}
}
