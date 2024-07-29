#pragma once

#include <format>

#ifdef NDEBUG
#define BS_ASSERT(Expression, Format, ...) ((void)0)
#else
#define BS_ASSERT(Expression, Format, ...) 								   \
if (!(Expression))														   \
{																		   \
	printf("\x1B[91mAssert failed: \n%s; \n%s; \n%s; \nLine: %u\033[0m\n", \
		#Expression,													   \
		std::format(Format, __VA_ARGS__).c_str(),						   \
		__FILE__,														   \
		(unsigned)(__LINE__));											   \
	DEBUGBREAK;															   \
}
#endif
