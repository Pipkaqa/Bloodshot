#pragma once

#include "Logging/LoggingMacros.h"
#include "Platform/Platform.h"

#include <format>

#ifdef BS_DEBUG
#define BS_ASSERT(Expression, Format, ...) \
    UNLIKELY if (!(Expression)) \
    { \
        BS_LOG(Fatal, "{}, {}, {}, {}", #Expression, __FILE__, __LINE__, std::format(Format, ##__VA_ARGS__)); \
        BS_DEBUG_BREAK(); \
    }
#else
#define BS_ASSERT(Expression, Format, ...)
#endif
