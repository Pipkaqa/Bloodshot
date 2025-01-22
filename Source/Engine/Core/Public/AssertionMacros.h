#pragma once

#include "Platform/Platform.h"
#include "Logging/LoggingMacros.h"

#include <format>

#ifdef BS_DEBUG
    #define BS_ASSERT(Expression, Format, ...)
#else
    #define BS_ASSERT(Expression, Format, ...)                                     \
        if (!(Expression))                                                         \
        {                                                                          \
            printf("\x1B[91mAssert failed: \n%s; \n%s; \n%s; \nLine: %u\033[0m\n", \
                #Expression,                                                       \
                std::format(Format, __VA_ARGS__).c_str(),                          \
                __FILE__,                                                          \
                (unsigned)(__LINE__));                                             \
                BS_DEBUG_BREAK();                                                  \
        }
#endif

#define BS_CHECK(Expression, Format, ...)   \
    if (!Expression)                        \
    {                                       \
        BS_LOG(Fatal, Format, __VA_ARGS__); \
    }
