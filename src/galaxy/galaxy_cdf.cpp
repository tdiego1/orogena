/**************************************************************************************************/
/**
 * @file galaxy_cdf.cpp
 * @brief Implementation of CumulativeDistributionFunction
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

#include "galaxy_cdf.h"

#include <algorithm>

#include <cmath>
#include <stdexcept>

namespace Orogena::Galaxy
{

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
void CumulativeDistributionFunction::SetupRealistic(float64_t maxIntensity,
                                                    float64_t k,
                                                    float64_t diskScaleLength,
                                                    float64_t bulgeRadius,
                                                    float64_t min,
                                                    float64_t max,
                                                    size_t    numPoints)
{
    // Clear existing data
    m_X.clear();
    m_Y.clear();

    // Reserve space for efficiency
    const size_t numSteps = numPoints / 2U; // Simpson's rule uses pairs
    m_X.reserve(numSteps + 1U);
    m_Y.reserve(numSteps + 1U);

    // Build the cumulative distribution function using Simpson's rule
    const float64_t h = (max - min) / (float64_t)numPoints;
    float64_t       x = 0.0;
    float64_t       y = 0.0;

    // First point at x=0
    m_Y.push_back(0.0);
    m_X.push_back(0.0);

    // Integrate using Simpson's rule: ∫f(x)dx ≈ h/3 * [f(x₀) + 4f(x₁) + f(x₂)]
    for (uint32_t i = 0; i < numPoints; i += 2)
    {
        const float64_t x0 = min + i * h;
        const float64_t x1 = min + (i + 1) * h;
        const float64_t x2 = min + (i + 2) * h;

        const float64_t f0 = IntensityProfile(x0, maxIntensity, k, diskScaleLength, bulgeRadius);
        const float64_t f1 = IntensityProfile(x1, maxIntensity, k, diskScaleLength, bulgeRadius);
        const float64_t f2 = IntensityProfile(x2, maxIntensity, k, diskScaleLength, bulgeRadius);

        y += (h / 3.0) * (f0 + 4.0 * f1 + f2);

        x = x2;
        m_X.push_back(x);
        m_Y.push_back(y);
    }

    // Normalize CDF to [0, 1]
    const float64_t totalIntegral = m_Y.back();
    if (totalIntegral > 0.0)
    {
        for (auto& val : m_Y)
        {
            val /= totalIntegral;
        }
    }
    else
    {
        throw std::runtime_error("CDF: Total integral is zero - invalid intensity profile");
    }
}

/**************************************************************************************************/
float64_t CumulativeDistributionFunction::ValFromProb(float64_t probability) const
{
    if (m_X.empty() || m_Y.empty())
    {
        throw std::runtime_error("CDF not initialized - call SetupRealistic() first");
    }

    if (probability < 0.0 || probability > 1.0)
    {
        throw std::out_of_range("Probability must be in range [0, 1]");
    }

    // Edge cases
    if (probability <= 0.0)
    {
        return m_X.front();
    }
    if (probability >= 1.0)
    {
        return m_X.back();
    }

    // Binary search for the interval containing the probability
    // std::lower_bound finds first element >= probability
    auto it = std::lower_bound(m_Y.begin(), m_Y.end(), probability);

    if (it == m_Y.end())
    {
        return m_X.back();
    }

    // Get index
    const size_t idx = std::distance(m_Y.begin(), it);

    if (idx == 0)
    {
        return m_X[0];
    }

    // Linear interpolation between adjacent points
    // y = y0 + m * (x - x0)  =>  x = x0 + (y - y0) / m
    const float64_t y0 = m_Y[idx - 1];
    const float64_t y1 = m_Y[idx];
    const float64_t x0 = m_X[idx - 1];
    const float64_t x1 = m_X[idx];

    // Calculate slope (avoid division by zero)
    const float64_t dy = y1 - y0;
    if (std::abs(dy) < 1e-10)
    {
        return x0;
    }

    // Linear interpolation
    const float64_t t = (probability - y0) / dy;
    return x0 + t * (x1 - x0);
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
float64_t CumulativeDistributionFunction::IntensityProfile(float64_t radius,
                                                           float64_t maxIntensity,
                                                           float64_t k,
                                                           float64_t diskScaleLength,
                                                           float64_t bulgeRadius) const
{
    // Composite intensity profile: bulge + disk
    // Bulge: I_bulge(r) = I₀ * exp(-k * r^0.25)     [Sérsic n=4 approximation]
    // Disk:  I_disk(r)  = I₀ * exp(-r / a)          [Exponential disk]

    if (radius < bulgeRadius)
    {
        // Bulge-dominated region: exponential of r^0.25 (Sérsic profile approximation)
        return maxIntensity * std::exp(-k * std::pow(radius, 0.25));
    }
    else
    {
        // Disk-dominated region: exponential decay
        // Start from bulge edge intensity to ensure continuity
        const float64_t bulgeBorderIntensity =
            maxIntensity * std::exp(-k * std::pow(bulgeRadius, 0.25));

        const float64_t diskRadius = radius - bulgeRadius;
        return bulgeBorderIntensity * std::exp(-diskRadius / diskScaleLength);
    }
}

} // namespace Orogena::Galaxy
