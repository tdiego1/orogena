/**************************************************************************************************/
/**
 * @file math_constants.h
 * @brief Mathematical and physical constants for worldbuilding calculations.
 *
 * @details
 * Defines fundamental mathematical constants and physical unit conversions used throughout the
 * Orogena worldbuilding suite. All constants are constexpr for compile-time
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

#include "utils/utils_types.h"

namespace Orogena::Math
{

//=================================================================================================
// Mathematical Constants
//=================================================================================================

/// @brief Pi constant (π)
constexpr float64_t c_Pi = 3.14159265358979323846;

/// @brief Two times Pi (2π)
constexpr float64_t c_TwoPi = 6.28318530717958647692;

/// @brief Half Pi (π/2)
constexpr float64_t c_HalfPi = 1.57079632679489661923;

/// @brief Pi squared (π²)
constexpr float64_t c_PiSquared = 9.86960440108935861883;

/// @brief Euler's number (e)
constexpr float64_t c_E = 2.71828182845904523536;

/// @brief Golden ratio (φ)
constexpr float64_t c_GoldenRatio = 1.61803398874989484820;

/// @brief Square root of 2
constexpr float64_t c_Sqrt2 = 1.41421356237309504880;

/// @brief Square root of 3
constexpr float64_t c_Sqrt3 = 1.73205080756887729352;

//=================================================================================================
// Angular Conversions
//=================================================================================================

/// @brief Degrees to radians conversion factor (π/180)
constexpr float64_t c_DegToRad = 0.017453292519943295;

/// @brief Radians to degrees conversion factor (180/π)
constexpr float64_t c_RadToDeg = 57.29577951308232;

//=================================================================================================
// Astronomical & Physical Constants
//=================================================================================================

/// @brief Parsecs to kilometers (1 pc = 3.086e13 km)
constexpr float64_t c_PcToKm = 3.08567758129e13;

/// @brief Kilometers to parsecs
constexpr float64_t c_KmToPc = 3.24077929e-14;

/// @brief Astronomical Unit to kilometers (1 AU = 1.496e8 km)
constexpr float64_t c_AuToKm = 1.495978707e8;

/// @brief Kilometers to Astronomical Units
constexpr float64_t c_KmToAu = 6.68458712e-9;

/// @brief Light year to kilometers (1 ly = 9.461e12 km)
constexpr float64_t c_LyToKm = 9.4607304725808e12;

/// @brief Kilometers to light years
constexpr float64_t c_KmToLy = 1.0570008340246e-13;

/// @brief Gravitational constant (m³ kg⁻¹ s⁻²)
constexpr float64_t c_GravitationalConstant = 6.67430e-11;

/// @brief Speed of light in vacuum (km/s)
constexpr float64_t c_SpeedOfLight = 299792.458;

//=================================================================================================
// Time Conversions
//=================================================================================================

/// @brief Seconds per minute
constexpr float64_t c_SecPerMinute = 60.0;

/// @brief Seconds per hour
constexpr float64_t c_SecPerHour = 3600.0;

/// @brief Seconds per day (24 hours)
constexpr float64_t c_SecPerDay = 86400.0;

/// @brief Seconds per year (365.25 days, accounting for leap years)
constexpr float64_t c_SecPerYear = 31557600.0; // 365.25 * 86400

/// @brief Days per year (average)
constexpr float64_t c_DaysPerYear = 365.25;

/// @brief Hours per day
constexpr float64_t c_HoursPerDay = 24.0;

/// @brief Minutes per hour
constexpr float64_t c_MinutesPerHour = 60.0;

//=================================================================================================
// Earth Reference Values
//=================================================================================================

/// @brief Earth's mass (kg)
constexpr float64_t c_EarthMass = 5.972e24;

/// @brief Earth's radius (km)
constexpr float64_t c_EarthRadius = 6371.0;

/// @brief Earth's surface gravity (m/s²)
constexpr float64_t c_EarthGravity = 9.80665;

/// @brief Earth's rotation period (hours)
constexpr float64_t c_EarthRotationHours = 23.9344694;

/// @brief Earth's orbital period (days)
constexpr float64_t c_EarthOrbitalDays = 365.256363004;

/// @brief Earth's axial tilt (degrees)
constexpr float64_t c_EarthAxialTilt = 23.4392811;

/// @brief Earth's orbital radius (AU)
constexpr float64_t c_EarthOrbitalRadius = 1.0;

/// @brief Solar mass (kg)
constexpr float64_t c_SolarMass = 1.98892e30;

/// @brief Solar luminosity (watts)
constexpr float64_t c_SolarLuminosity = 3.828e26;

/// @brief Solar radius (km)
constexpr float64_t c_SolarRadius = 695700.0;

//=================================================================================================
// Tolerance Values
//=================================================================================================

/// @brief Default epsilon for floating-point comparisons (double precision)
constexpr float64_t c_Epsilon = 1e-10;

/// @brief Epsilon for single-precision comparisons
constexpr float32_t c_EpsilonF = 1e-6f;

/// @brief Small value threshold
constexpr float64_t c_SmallValue = 1e-8;

} // namespace Orogena::Math
