#pragma once

#include "Platform/Platform.h"
#include "String/Format.h"

#include <exception>
#include <stdio.h>

#ifdef BS_DEBUG
#define BS_ASSERT(InExpression, InFormat, ...) \
    UNLIKELY if (!(InExpression)) \
    { \
        ::Bloodshot::Private::String::FLowLevelString Str = ::Bloodshot::Private::String::LLFormat(InFormat, ##__VA_ARGS__); \
        printf("\x1B[91mAssert failed: \n%s; \n%s; \n%s; \nLine: %u\033[0m\n", \
            #InExpression, \
            Str.Data, \
            __FILE__, \
            (unsigned)(__LINE__)); \
        Str.Release(); \
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
