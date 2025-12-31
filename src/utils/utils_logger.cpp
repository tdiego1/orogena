/**************************************************************************************************/
/**
 * @file utils_logger.cpp
 * @brief Implementation of Logger
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "utils/utils_logger.h"

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <chrono>
#include <format>

namespace Orogena::Utils
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

std::shared_ptr<spdlog::logger> Logger::s_Logger;
std::shared_ptr<spdlog::sinks::sink> Logger::s_ConsoleSink;
std::shared_ptr<spdlog::sinks::sink> Logger::s_FileSink;

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

//=================================================================================================
// Public Functions
//=================================================================================================

void Logger::Initialize()
{
    // Create console sink
    s_ConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    s_ConsoleSink->set_level(spdlog::level::debug);

    // Generate timestamped log filename
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    std::string log_filename =
        std::format("orogena_{:04d}-{:02d}-{:02d}_{:02d}-{:02d}-{:02d}.log",
                    local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
                    local_time.tm_hour, local_time.tm_min, local_time.tm_sec);

    // Create file sink with rotation (10MB, 3 files)
    s_FileSink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_filename, 1024 * 1024 * 10, 3);
    s_FileSink->set_level(spdlog::level::trace);

    // Create logger with both sinks
    std::vector<spdlog::sink_ptr> sinks{s_ConsoleSink, s_FileSink};
    s_Logger = std::make_shared<spdlog::logger>("orogena", sinks.begin(), sinks.end());
    s_Logger->set_level(spdlog::level::debug);

    Info("Orogena logger initialized");
}

void Logger::Shutdown()
{
    if (s_Logger)
    {
        s_Logger->info("Orogena logger shutting down");
        s_Logger->flush();
        s_Logger.reset();
    }
    s_FileSink.reset();
    s_ConsoleSink.reset();
}

bool Logger::IsInitialized()
{
    return s_Logger != nullptr;
}

void Logger::SetLevel(LogLevel level)
{
    s_Logger->set_level(ToSpdlogLevel(level));
}

void Logger::SetConsoleLevel(LogLevel level)
{
    s_ConsoleSink->set_level(ToSpdlogLevel(level));
}

void Logger::SetFileLevel(LogLevel level)
{
    s_FileSink->set_level(ToSpdlogLevel(level));
}

//=================================================================================================
// Private Functions
//=================================================================================================

spdlog::level::level_enum Logger::ToSpdlogLevel(LogLevel level)
{
    switch (level)
    {
        case LogLevel::TRACE:
            return spdlog::level::trace;
        case LogLevel::DEBUG:
            return spdlog::level::debug;
        case LogLevel::INFO:
            return spdlog::level::info;
        case LogLevel::WARN:
            return spdlog::level::warn;
        case LogLevel::ERROR:
            return spdlog::level::err;
        case LogLevel::CRITICAL:
            return spdlog::level::critical;
        case LogLevel::OFF:
            return spdlog::level::off;
        default:
            return spdlog::level::info;
    }
}

} // namespace Orogena::Utils
