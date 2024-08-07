#pragma once

#include "Logger.h"
#include "LogLevel.h"

#define BS_LOG(Level, Format, ...) ::Bloodshot::ILogger::Log<::Bloodshot::ELogLevel::Level>(Format, __VA_ARGS__)
#define BS_LOG_IF(Expression, Level, Format, ...)                                      \
    if (Expression)                                                                    \
    {                                                                                  \
        ::Bloodshot::ILogger::Log<::Bloodshot::ELogLevel::Level>(Format, __VA_ARGS__); \
    }
