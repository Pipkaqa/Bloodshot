#pragma once

#include "Logger.h"
#include "LogLevel.h"

#define BS_LOG(Level, Format, ...) ::Bloodshot::FLogger::Log<::Bloodshot::ELogLevel::Level>(Format, __VA_ARGS__)
#define BS_LOG_IF(Expression, Level, Format, ...)                                          \
    do                                                                                     \
    {                                                                                      \
        [[unlikely]] if (Expression)                                                       \
        {                                                                                  \
            ::Bloodshot::FLogger::Log<::Bloodshot::ELogLevel::Level>(Format, __VA_ARGS__); \
        }                                                                                  \
    } while(0);
