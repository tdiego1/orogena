/**************************************************************************************************/
/**
 * @file galaxy_types.cpp
 * @brief Implementation of galaxy types
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

#include "galaxy_types.h"

#include "math/math_constants.h"

namespace Orogena::Galaxy
{

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
void StarData::CalculatePosition(int32_t perturbationN, float64_t perturbationAmp)
{
    const float64_t alpha = theta * Math::c_DegToRad;
    const float64_t cosAlpha = std::cos(alpha);
    const float64_t sinAlpha = std::sin(alpha);
    const float64_t cosBeta = std::cos(-angle);
    const float64_t sinBeta = std::sin(-angle);

    // Calculate elliptical orbit position
    glm::dvec2 newPos{
        center.x + (semiMajorAxis * cosAlpha * cosBeta - semiMinorAxis * sinAlpha * sinBeta),
        center.y + (semiMajorAxis * cosAlpha * sinBeta + semiMinorAxis * sinAlpha * cosBeta)};

    // Add spiral perturbations if enabled
    if (perturbationAmp > 0.0 && perturbationN > 0)
    {
        const float64_t perturbation = semiMajorAxis / perturbationAmp;
        newPos.x += perturbation * std::sin(alpha * 2.0 * perturbationN);
        newPos.y += perturbation * std::cos(alpha * 2.0 * perturbationN);
    }

    position = newPos;
}

} // namespace Orogena::Galaxy
