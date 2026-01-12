/**************************************************************************************************/
/**
 * @file galaxy_types.h
 * @brief Core types for galaxy simulation
 *
 * @details
 * Defines types for galaxy simulation system.
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

#include <glm/glm.hpp>

#include "utils/utils_types.h"

namespace Orogena::Galaxy
{

//=================================================================================================
// Star Data Structure
//=================================================================================================

/**
 * @brief Represents a single star in the galaxy
 *
 * @details
 * Stores positional, orbital, and physical properties of a star.
 * Uses elliptical orbit parameters for realistic spiral structure.
 */
struct StarData
{
    float64_t theta{0.0};          ///< Position on ellipse (degrees)
    float64_t velTheta{0.0};       ///< Angular velocity (degrees/year)
    float64_t angle{0.0};          ///< Tilt of elliptical orbit (radians)
    float64_t semiMajorAxis{0.0};  ///< Semi-major axis (parsecs)
    float64_t semiMinorAxis{0.0};  ///< Semi-minor axis (parsecs)
    float64_t temperature{6000.0}; ///< Star temperature (Kelvin)
    float64_t magnitude{1.0};      ///< Brightness magnitude

    glm::dvec2 center{0.0, 0.0};   ///< Center of elliptical orbit
    glm::dvec2 position{0.0, 0.0}; ///< Current Cartesian position
    glm::dvec2 velocity{0.0, 0.0}; ///< Current velocity vector

    /**
     * @brief Calculate Cartesian position from orbital parameters
     *
     * @param perturbationN Number of spiral arms (0 for no perturbation)
     * @param perturbationAmp Amplitude of perturbation
     */
    void CalculatePosition(uint32_t perturbationN, float64_t perturbationAmp);
};

//=================================================================================================
// Galaxy Configuration
//=================================================================================================

/**
 * @brief Configuration parameters for galaxy generation
 */
struct GalaxyConfig
{
    float64_t radius{13000.0};         ///< Galaxy radius (parsecs)
    float64_t coreRadius{4000.0};      ///< Core radius (parsecs)
    float64_t angularOffset{0.0004};   ///< Angular offset per parsec
    float64_t excentricityInner{0.85}; ///< Inner orbit excentricity
    float64_t excentricityOuter{0.95}; ///< Outer orbit excentricity
    float64_t sigma{0.5};              ///< Star distribution parameter
    float64_t velocityInner{200.0};    ///< Velocity at core edge (km/s)
    float64_t velocityOuter{300.0};    ///< Velocity at disk edge (km/s)

    uint32_t  numStars{30000};       ///< Total number of stars
    uint32_t  perturbationN{2};      ///< Number of spiral arms
    float64_t perturbationAmp{40.0}; ///< Spiral perturbation amplitude
    float64_t dustRenderSize{100.0}; ///< Visual size of dust particles

    bool hasDarkMatter{true}; ///< Include dark matter in physics
};

} // namespace Orogena::Galaxy
