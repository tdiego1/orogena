/**************************************************************************************************/
/**
 * @file math_interpolation.h
 * @brief Interpolation and blending functions
 *
 * @details
 * Provides various interpolation methods used throughout worldbuilding calculations, including
 * linear, cubic, and piecewise interpolation.
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres
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

#include <vector>

#include "utils/utils_types.h"

namespace Orogena::Math
{

//=================================================================================================
// Linear Interpolation
//=================================================================================================

/**
 * @brief Linear interpolation between two values
 *
 * @param t Interpolation parameter [0, 1]
 * @param a Start value (when t=0)
 * @param b End value (when t=1)
 * @return Interpolated value
 *
 * @note No clamping - t can be outside [0, 1] for extrapolation
 */
constexpr float64_t Lerp(float64_t t, float64_t a, float64_t b)
{
    return a + t * (b - a);
}

/**
 * @brief Linear interpolation between two values (single precision)
 */
constexpr float32_t Lerp(float32_t t, float32_t a, float32_t b)
{
    return a + t * (b - a);
}

/**
 * @brief Inverse linear interpolation (find t for given value)
 *
 * @param value The value to find parameter for
 * @param a Start value
 * @param b End value
 * @return Parameter t such that Lerp(t, a, b) = value
 */
constexpr float64_t InverseLerp(float64_t value, float64_t a, float64_t b)
{
    return (value - a) / (b - a);
}

//=================================================================================================
// Smooth Interpolation
//=================================================================================================

/**
 * @brief Smooth step interpolation (cubic Hermite)
 *
 * @details
 * Produces smooth S-curve interpolation with zero derivatives at endpoints.
 * Uses formula: 3t² - 2t³
 *
 * @param t Interpolation parameter [0, 1]
 * @return Smoothed value [0, 1]
 */
constexpr float64_t SmoothStep(float64_t t)
{
    t = t * t * (3.0 - 2.0 * t);
    return t;
}

/**
 * @brief Smoother step interpolation (quintic)
 *
 * @details
 * Even smoother than SmoothStep, with zero first and second derivatives at endpoints.
 * Uses formula: 6t⁵ - 15t⁴ + 10t³
 *
 * @param t Interpolation parameter [0, 1]
 * @return Smoothed value [0, 1]
 */
constexpr float64_t SmootherStep(float64_t t)
{
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

//=================================================================================================
// Bilinear Interpolation
//=================================================================================================

/**
 * @brief Bilinear interpolation for 2D grids
 *
 * @param v00 Value at (0, 0)
 * @param v10 Value at (1, 0)
 * @param v01 Value at (0, 1)
 * @param v11 Value at (1, 1)
 * @param tx Interpolation parameter in x [0, 1]
 * @param ty Interpolation parameter in y [0, 1]
 * @return Interpolated value
 */
constexpr float64_t BilinearInterp(
    float64_t v00, float64_t v10, float64_t v01, float64_t v11, float64_t tx, float64_t ty)
{
    float64_t nx0 = Lerp(tx, v00, v10);
    float64_t nx1 = Lerp(tx, v01, v11);
    return Lerp(ty, nx0, nx1);
}

//=================================================================================================
// Piecewise Linear Interpolation
//=================================================================================================

/**
 * @brief Piecewise linear interpolation through multiple points
 *
 * @details
 * Given a sorted list of (x, y) control points, finds the interpolated y value
 * for any given x using linear interpolation between adjacent points.
 *
 * Example:
 * @code
 * std::vector<std::pair<float64_t, float64_t>> points = {
 *     {0.0, 1.0},    // At core center: excentricity = 1.0
 *     {6000.0, 0.8}, // At core edge: excentricity = 0.8
 *     {15000.0, 1.0} // At galaxy edge: excentricity = 1.0
 * };
 * float64_t ex = PiecewiseLinear(8000.0, points); // Interpolated value
 * @endcode
 *
 * @param x Input value
 * @param points Sorted array of (x, y) pairs (must be sorted by x ascending)
 * @return Interpolated y value
 *
 * @throws std::invalid_argument if points is empty or not sorted
 * @note If x is outside range, returns nearest endpoint value (clamping)
 */
float64_t PiecewiseLinear(float64_t x, const std::vector<std::pair<float64_t, float64_t>>& points);

//=================================================================================================
// Clamping Utilities
//=================================================================================================

/**
 * @brief Clamp value to range [min, max]
 *
 * @param value Value to clamp
 * @param min Minimum allowed value
 * @param max Maximum allowed value
 * @return Clamped value
 */
template <typename T> constexpr T Clamp(T value, T min, T max)
{
    return value < min ? min : (value > max ? max : value);
}

/**
 * @brief Clamp value to [0, 1]
 */
template <typename T> constexpr T Clamp01(T value)
{
    return Clamp(value, static_cast<T>(0), static_cast<T>(1));
}

//=================================================================================================
// Remapping
//=================================================================================================

/**
 * @brief Remap value from one range to another
 *
 * @param value Input value in range [inMin, inMax]
 * @param inMin Input range minimum
 * @param inMax Input range maximum
 * @param outMin Output range minimum
 * @param outMax Output range maximum
 * @return Remapped value in range [outMin, outMax]
 *
 * Example: Remap(0.5, 0.0, 1.0, 100.0, 200.0) = 150.0
 */
constexpr float64_t
Remap(float64_t value, float64_t inMin, float64_t inMax, float64_t outMin, float64_t outMax)
{
    float64_t t = InverseLerp(value, inMin, inMax);
    return Lerp(t, outMin, outMax);
}

} // namespace Orogena::Math
