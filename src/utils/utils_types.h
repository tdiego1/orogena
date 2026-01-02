/**************************************************************************************************/
/**
 * @file utils_types.h
 * @brief Standard type definitions used throughout Orogena
 *
 * @details
 * Provides consistent integer and floating-point type aliases using standard C++ fixed-width types.
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * For commercial licensing options, please contact: tdiego001@gmail.com
 */
/**************************************************************************************************/

#pragma once

#include <cstdint>

//=============================================================================================
// Public Types
//=============================================================================================

// Character types
using char_t = char;

// Fixed-width integer types (from <cstdint>)
using int8_t = std::int8_t;
using int16_t = std::int16_t;
using int32_t = std::int32_t;
using int64_t = std::int64_t;
using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;

// Floating-point types
using float32_t = float;
using float64_t = double;

namespace Orogena::Utils
{
/**
 * @brief RGB color structure
 */
struct ColorRGBF
{
    float32_t r; ///< Red
    float32_t g; ///< Green
    float32_t b; ///< Blue
};

} // namespace Orogena::Utils