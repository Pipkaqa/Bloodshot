#pragma once

#include "Platform/Platform.h"

#include <format>
#include <source_location>
#include <stdio.h>

#ifdef BS_DEBUG
#define BS_ASSERT(InExpression, InFormat, ...) \
    UNLIKELY if (!(InExpression)) \
    { \
        const std::source_location SourceLocation = std::source_location::current(); \
        printf("\x1B[91m[%s:%u - %s]: %s (Expression: %s)\033[0m\n", \
        SourceLocation.file_name(), \
        SourceLocation.line(), \
        SourceLocation.function_name(), \
        std::format(InFormat, ##__VA_ARGS__).c_str(), \
        #InExpression); \
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
