/**************************************************************************************************/
/**
 * @file math_orbital.h
 * @brief Orbital mechanics calculations
 *
 * @details
 * Provides functions for calculating orbital parameters, velocities, and periods using Newtonian
 * gravity. Used by both stellar and galaxy modules.
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
// Orbital Velocity
//=================================================================================================

/**
 * @brief Calculate orbital velocity using vis-viva equation
 *
 * @details
 * Uses the vis-viva equation: v² = GM(2/r - 1/a)
 * For circular orbits (a = r): v = sqrt(GM/r)
 *
 * @param radius Distance from center of mass (meters)
 * @param centralMass Mass of central body (kilograms)
 * @return Orbital velocity (meters/second)
 *
 * @note For galaxy simulations, use CalculateGalacticOrbitalVelocity() instead
 */
float64_t CalculateOrbitalVelocity(float64_t radius, float64_t centralMass);

/**
 * @brief Calculate orbital velocity in km/s
 *
 * @param radiusKm Distance from center (kilometers)
 * @param centralMassSolar Mass in solar masses
 * @return Orbital velocity (km/s)
 */
float64_t CalculateOrbitalVelocityKmS(float64_t radiusKm, float64_t centralMassSolar);

//=================================================================================================
// Orbital Period
//=================================================================================================

/**
 * @brief Calculate orbital period from radius and velocity
 *
 * @details
 * Uses: T = 2πr / v
 *
 * @param radiusKm Orbital radius (kilometers)
 * @param velocityKmS Orbital velocity (km/s)
 * @return Orbital period (years)
 */
float64_t CalculateOrbitalPeriod(float64_t radiusKm, float64_t velocityKmS);

/**
 * @brief Calculate orbital period using Kepler's third law
 *
 * @details
 * Uses: T² = (4π²/GM) * a³
 * Where a is the semi-major axis
 *
 * @param semiMajorAxisAU Semi-major axis (Astronomical Units)
 * @param centralMassSolar Central mass (solar masses)
 * @return Orbital period (years)
 */
float64_t CalculateKeplerPeriod(float64_t semiMajorAxisAU, float64_t centralMassSolar);

//=================================================================================================
// Angular Velocity
//=================================================================================================

/**
 * @brief Convert linear orbital velocity to angular velocity
 *
 * @param velocityKmS Linear velocity (km/s)
 * @param radiusKm Orbital radius (km)
 * @return Angular velocity (degrees/year)
 */
float64_t VelocityToAngularVelocity(float64_t velocityKmS, float64_t radiusKm);

/**
 * @brief Convert angular velocity to linear velocity
 *
 * @param angularVelDegPerYear Angular velocity (degrees/year)
 * @param radiusKm Orbital radius (km)
 * @return Linear velocity (km/s)
 */
float64_t AngularVelocityToVelocity(float64_t angularVelDegPerYear, float64_t radiusKm);

//=================================================================================================
// Escape Velocity
//=================================================================================================

/**
 * @brief Calculate escape velocity from surface of body
 *
 * @details
 * Uses: v_esc = sqrt(2GM/r)
 *
 * @param radiusKm Body radius (kilometers)
 * @param massKg Body mass (kilograms)
 * @return Escape velocity (km/s)
 */
float64_t CalculateEscapeVelocity(float64_t radiusKm, float64_t massKg);

//=================================================================================================
// Hill Sphere
//=================================================================================================

/**
 * @brief Calculate Hill sphere radius (gravitational sphere of influence)
 *
 * @details
 * For a body orbiting a primary, the Hill sphere is the region where the body's
 * gravity dominates over the primary's tidal forces.
 * Formula: r_H = a * (m / 3M)^(1/3)
 *
 * @param orbitalRadiusKm Distance from primary (km)
 * @param bodyMass Mass of orbiting body (kg)
 * @param primaryMass Mass of primary body (kg)
 * @return Hill sphere radius (km)
 */
float64_t CalculateHillSphere(float64_t orbitalRadiusKm, float64_t bodyMass, float64_t primaryMass);

} // namespace Orogena::Math
