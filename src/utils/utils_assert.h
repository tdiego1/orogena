/**************************************************************************************************/
/**
 * @file utils_assert.h
 * @brief Wrapper for libassert
 *
 * @details
 * Import libassert macros into global namespace for convenience using lowecase variants
 * (enable with -DLIBASSERT_LOWERCASE if needed).
 * Otherwise use: DEBUG_ASSERT, ASSERT, ASSUME, PANIC, UNREACHABLE.
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

#include <format>
#include <libassert/assert.hpp>

namespace Orogena
{
namespace Utils
{

// Custom stringification for Orogena types
// Add as needed during development.

} // namespace Utils
} // namespace Orogena
