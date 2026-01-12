/**************************************************************************************************/
/**
 * @file math_orbital.cpp
 * @brief Implementation of orbital mechanics
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

#include "math_orbital.h"

#include "math_constants.h"

#include <cmath>

namespace Orogena::Math
{

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
float64_t CalculateOrbitalVelocity(float64_t radius, float64_t centralMass)
{
    // v = sqrt(GM/r)
    return std::sqrt(c_GravitationalConstant * centralMass / radius);
}

/**************************************************************************************************/
float64_t CalculateOrbitalVelocityKmS(float64_t radiusKm, float64_t centralMassSolar)
{
    // Convert to SI units
    float64_t radiusM = radiusKm * 1000.0;
    float64_t massKg = centralMassSolar * c_SolarMass;

    // Calculate velocity in m/s
    float64_t velocityMS = CalculateOrbitalVelocity(radiusM, massKg);

    // Convert to km/s
    return velocityMS / 1000.0;
}

/**************************************************************************************************/
float64_t CalculateOrbitalPeriod(float64_t radiusKm, float64_t velocityKmS)
{
    // Circumference
    float64_t circumferenceKm = c_TwoPi * radiusKm;

    // Period in seconds
    float64_t periodSeconds = circumferenceKm / velocityKmS;

    // Convert to years
    return periodSeconds / c_SecPerYear;
}

/**************************************************************************************************/
float64_t CalculateKeplerPeriod(float64_t semiMajorAxisAU, float64_t centralMassSolar)
{
    // T² = (4π²/GM) * a³
    // Simplified for solar masses and AU: T² ≈ a³/M

    float64_t a3 = semiMajorAxisAU * semiMajorAxisAU * semiMajorAxisAU;
    float64_t t2 = a3 / centralMassSolar;

    return std::sqrt(t2);
}

/**************************************************************************************************/
float64_t VelocityToAngularVelocity(float64_t velocityKmS, float64_t radiusKm)
{
    // Circumference
    float64_t circumferenceKm = c_TwoPi * radiusKm;

    // Period in years
    float64_t periodYears = circumferenceKm / (velocityKmS * c_SecPerYear);

    // Angular velocity in degrees/year
    return 360.0 / periodYears;
}

/**************************************************************************************************/
float64_t AngularVelocityToVelocity(float64_t angularVelDegPerYear, float64_t radiusKm)
{
    // Period in years from angular velocity
    float64_t periodYears = 360.0 / angularVelDegPerYear;

    // Circumference
    float64_t circumferenceKm = c_TwoPi * radiusKm;

    // Velocity in km/s
    return circumferenceKm / (periodYears * c_SecPerYear);
}

/**************************************************************************************************/
float64_t CalculateEscapeVelocity(float64_t radiusKm, float64_t massKg)
{
    // v_esc = sqrt(2GM/r)
    float64_t radiusM = radiusKm * 1000.0;
    float64_t velocityMS = std::sqrt(2.0 * c_GravitationalConstant * massKg / radiusM);

    return velocityMS / 1000.0; // Convert to km/s
}

/**************************************************************************************************/
float64_t CalculateHillSphere(float64_t orbitalRadiusKm, float64_t bodyMass, float64_t primaryMass)
{
    // r_H = a * (m / 3M)^(1/3)
    float64_t massRatio = bodyMass / (3.0 * primaryMass);
    return orbitalRadiusKm * std::cbrt(massRatio);
}

} // namespace Orogena::Math
