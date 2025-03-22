#pragma once

#include "Platform/Platform.h"

#include <format>
#include <stdio.h>

#ifdef BS_DEBUG
#define BS_ASSERT(InExpression, InFormat, ...) \
    UNLIKELY if (!(InExpression)) \
    { \
        printf("\x1B[91mAssert failed: \n%s; \n%s; \n%s; \nLine: %u\033[0m\n", \
        #InExpression, \
        std::format(InFormat, ##__VA_ARGS__).c_str(), \
        __FILE__, \
        (unsigned)(__LINE__)); \
        BS_DEBUG_BREAK(); \
        BS_TERMINATE(); \
    }
#define BS_CHECK(InExpression) \
    UNLIKELY if(!(InExpression)) \
    { \
        BS_DEBUG_BREAK(); \
        BS_TERMINATE(); \
    }
#else
#define BS_ASSERT(InExpression, InFormat, ...)
#define BS_CHECK(InExpression)
#endif
