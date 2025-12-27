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
 * @brief Centralized logging facility
 *
 * @details
 * Wrapper around spdlog for application-wide logging.
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
     * @brief Gets the logger instance.
     */
    static std::shared_ptr<spdlog::logger> Get();

  private:
    //=============================================================================================
    // Private Members
    //=============================================================================================

    static std::shared_ptr<spdlog::logger> s_Logger; ///< Shared logger instance
};

} // namespace Orogena::Utils