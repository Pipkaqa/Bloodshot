#pragma once

#include "Logger.h"
#include "LogLevel.h"
#include "Platform/Platform.h"

#define BS_LOG(Level, Format, ...) ::Bloodshot::FLogger::GetInstance().Log<::Bloodshot::ELogLevel::Level>(Format, ##__VA_ARGS__)
#define BS_LOG_IF(Expression, Level, Format, ...) \
    do \
    { \
        UNLIKELY if (Expression) \
        { \
            ::Bloodshot::FLogger::GetInstance().Log<::Bloodshot::ELogLevel::Level>(Format, ##__VA_ARGS__); \
        } \
    } while(0);
