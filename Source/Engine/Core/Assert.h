#pragma once

#include "Debug/Logger.h"

#include <assert.h>

#ifdef NDEBUG
#define FL_CORE_ASSERT(Expression, Message) ((void)0)
#else
#define FL_CORE_ASSERT(Expression, Message, ...) 			 \
if (!(Expression))											 \
{															 \
	FL_CORE_FATAL("Assert failed: {0}; {1}; {2}; Line: {3}", \
		#Expression,										 \
		std::format(Message, __VA_ARGS__),					 \
		__FILE__,											 \
		(unsigned)(__LINE__));								 \
	DEBUGBREAK;												 \
}
#endif
