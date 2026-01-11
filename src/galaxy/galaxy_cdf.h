/**************************************************************************************************/
/**
 * @file galaxy_cdf.h
 * @brief Cumulative distribution function for star placement
 *
 * @details
 * Detailed description of the class purpose and responsibilities.
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

namespace Orogena::Galaxy
{

/**
 * @brief Cumulative distibution function for realistic star distribution
 *
 * @details
 * Used to generate star positions following a realistic density profile with bulge and disk
 * components
 */
class CumulativeDistributionFunction
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    CumulativeDistributionFunction() = default;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Setup realistic galaxy density profile
     *
     * @param maxIntensity Maximum intensity value
     * @param k Bulge parameter
     * @param diskScaleLength Scale length of disk
     * @param bulgeRadius Radius of bulge
     * @param min Minimum radius
     * @param max Maximum radius
     * @param numPoints Number of supporting points
     */
    void SetupRealistic(float64_t maxIntensity,
                        float64_t k,
                        float64_t diskScaleLength,
                        float64_t bulgeRadius,
                        float64_t min,
                        float64_t max,
                        size_t    numPoints);

    /**
     * @brief Get radius value from probability
     * @param probability Value in [0, 1]
     * @return Radius in parsecs
     */
    float64_t ValFromProb(float64_t probability) const;

    //=============================================================================================
    // Public Members
    //=============================================================================================

  private:
    //=============================================================================================
    // Private Types
    //=============================================================================================

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Intensity profile function
     *
     * @param radius Radius in parsecs
     * @param maxIntensity Maximum intensity value
     * @param k Bulge parameter
     * @param diskScaleLength Scale length of disk
     * @param bulgeRadius Radius of bulge
     * @return Intensity at given radius
     */
    float64_t IntensityProfile(float64_t radius,
                               float64_t maxIntensity,
                               float64_t k,
                               float64_t diskScaleLength,
                               float64_t bulgeRadius) const;

    //=============================================================================================
    // Private Members
    //=============================================================================================

    std::vector<float64_t> m_X; ///< Radius values
    std::vector<float64_t> m_Y; ///< Cumulative probabilities
};

} // namespace Orogena::Galaxy
