#include "LocalTime.h"

namespace Bloodshot
{
	char LocalTime::s_Buffer[128];

	const char* LocalTime::GetLocalTime()
	{
		time_t now = time(0);
		tm info;
		localtime_s(&info, &now);
		strftime(s_Buffer, 128, "[%Y-%m-%d] [%X]", &info);
		return s_Buffer;
	}
}
