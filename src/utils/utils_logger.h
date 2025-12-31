/**************************************************************************************************/
/**
 * @file utils_logger.h
 * @brief Logger utility using spdlog.
 *
 * @details
 * Provides centralized logging interface wrapper for spdlog.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <memory>

#include <spdlog/spdlog.h>

namespace Orogena::Utils
{

/**************************************************************************************************/
/**
 * @brief Log level enumeration
 */
enum class LogLevel : std::uint8_t
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    OFF
};

/**************************************************************************************************/
/**
 * @brief Centralized logging facility
 *
 * @details
 * Complete wrapper around spdlog for application-wide logging.
 * Provides abstraction from underlying logging implementation.
 */
class Logger
{
  public:
    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Initializes the logger.
     */
    static void Initialize();

    /**
     * @brief Sets the global log level.
     * @param level Minimum log level to output.
     */
    static void SetLevel(LogLevel level);

    /**
     * @brief Sets the console output log level.
     * @param level Minimum log level for console output.
     */
    static void SetConsoleLevel(LogLevel level);

    /**
     * @brief Sets the file output log level.
     * @param level Minimum log level for file output.
     */
    static void SetFileLevel(LogLevel level);

    //=============================================================================================
    // Logging Functions
    //=============================================================================================

    /**
     * @brief Log trace message (most verbose).
     * @tparam Args Variadic template for format arguments.
     * @param format Format string.
     * @param args Format arguments.
     */
    template <typename... Args>
    static void Trace(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        s_Logger->trace(format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log debug message.
     * @tparam Args Variadic template for format arguments.
     * @param format Format string.
     * @param args Format arguments.
     */
    template <typename... Args>
    static void Debug(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        s_Logger->debug(format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log info message.
     * @tparam Args Variadic template for format arguments.
     * @param format Format string.
     * @param args Format arguments.
     */
    template <typename... Args>
    static void Info(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        s_Logger->info(format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log warning message.
     * @tparam Args Variadic template for format arguments.
     * @param format Format string.
     * @param args Format arguments.
     */
    template <typename... Args>
    static void Warn(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        s_Logger->warn(format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log error message.
     * @tparam Args Variadic template for format arguments.
     * @param format Format string.
     * @param args Format arguments.
     */
    template <typename... Args>
    static void Error(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        s_Logger->error(format, std::forward<Args>(args)...);
    }

    /**
     * @brief Log critical message (least verbose, most severe).
     * @tparam Args Variadic template for format arguments.
     * @param format Format string.
     * @param args Format arguments.
     */
    template <typename... Args>
    static void Critical(spdlog::format_string_t<Args...> format, Args&&... args)
    {
        s_Logger->critical(format, std::forward<Args>(args)...);
    }

  private:
    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Converts LogLevel to spdlog level.
     * @param level Logger level enum.
     * @return Corresponding spdlog level.
     */
    static spdlog::level::level_enum ToSpdlogLevel(LogLevel level);

    //=============================================================================================
    // Private Members
    //=============================================================================================

    static std::shared_ptr<spdlog::logger> s_Logger;           ///< Shared logger instance
    static std::shared_ptr<spdlog::sinks::sink> s_ConsoleSink; ///< Console sink
    static std::shared_ptr<spdlog::sinks::sink> s_FileSink;    ///< File sink
};

} // namespace Orogena::Utils

// Convenience alias for shorter syntax
namespace Orogena
{
using Log = Utils::Logger;
} // namespace Orogena