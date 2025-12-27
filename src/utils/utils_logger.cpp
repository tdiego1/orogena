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

namespace Orogena::Utils
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

std::shared_ptr<spdlog::logger> Logger::s_Logger;

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

//=================================================================================================
// Public Functions
//=================================================================================================

void Logger::Initialize()
{
    // Create console sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    // Create file sink with rotation (10MB, 3 files)
    auto file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>("orogena.log", 1024 * 1024 * 10, 3);
    file_sink->set_level(spdlog::level::trace);

    // Create logger with both sinks
    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    s_Logger = std::make_shared<spdlog::logger>("orogena", sinks.begin(), sinks.end());
    s_Logger->set_level(spdlog::level::debug);

    // Set as default logger
    spdlog::set_default_logger(s_Logger);

    spdlog::info("Orogena logger initialized");
}

std::shared_ptr<spdlog::logger> Logger::Get()
{
    return s_Logger;
}

//=================================================================================================
// Private Functions
//=================================================================================================

} // namespace Orogena::Utils
