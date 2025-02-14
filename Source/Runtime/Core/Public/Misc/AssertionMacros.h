#pragma once

#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"

#include <format>

#ifdef BS_DEBUG
#define BS_ASSERT(Expression, Format, ...) \
    UNLIKELY if (!(Expression)) \
    { \
        BS_LOG(Fatal, "{}, {}, {}, {}", #Expression, __FILE__, __LINE__, std::format(Format, ##__VA_ARGS__)); \
    }
#define BS_CHECK(Expression) \
    UNLIKELY if(!(Expression)) \
    { \
        BS_DEBUG_BREAK(); \
        BS_TERMINATE(); \
    }
#else
#define BS_ASSERT(Expression, Format, ...)
#define BS_CHECK(Expression)
#endif
