/**************************************************************************************************/
/**
 * @file galaxy_model.cpp
 * @brief Implementation of Model
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

#include "galaxy_model.h"

#include "galaxy/galaxy_cdf.h"
#include "galaxy/galaxy_types.h"
#include "math/math_constants.h"
#include "math/math_orbital.h"
#include "utils/utils_logger.h"

#include <random>

namespace Orogena::Galaxy
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

/**************************************************************************************************/
Model::Model() : Model(GalaxyConfig{}) {}

/**************************************************************************************************/
Model::Model(const GalaxyConfig& config) : m_Config(config), m_FarFieldRadius(config.radius * 2.0)
{
    Reset();
}

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
void Model::Reset()
{
    Reset(m_Config);
}

/**************************************************************************************************/
void Model::Reset(const GalaxyConfig& config)
{
    m_Config = config;
    m_FarFieldRadius = m_Config.radius * 2.0;
    m_Time = 0.0;
    m_TimeStep = 0.0;

    m_RadialHistogram.fill(0);

    InitializeStars();

    Log::Info("Galaxy reset: {} stars, {} dust, radius={} pc", GetNumStars(), GetNumDust(),
              m_Config.radius);
}

/**************************************************************************************************/
void Model::ToggleDarkMatter()
{
    m_Config.hasDarkMatter = !m_Config.hasDarkMatter;
    Reset();
}

/**************************************************************************************************/
const glm::dvec2& Model::GetStarPosition(uint32_t index) const
{
    if (index < 0 || index >= GetNumStars())
    {
        throw std::out_of_range("Star index out of bounds");
    }
    return m_Stars[index].position;
}

/**************************************************************************************************/
float64_t Model::GetExcentricity(float64_t radius) const
{
    if (radius < m_Config.coreRadius)
    {
        // Core region: linearly increase from 1 to inner excentricity
        return 1.0 + (radius / m_Config.coreRadius) * (m_Config.excentricityInner - 1.0);
    }
    else if (radius <= m_Config.radius)
    {
        // Main disk: interpolate between inner and outer excentricity
        const float64_t t =
            (radius - m_Config.coreRadius) / (m_Config.radius - m_Config.coreRadius);
        return m_Config.excentricityInner +
               t * (m_Config.excentricityOuter - m_Config.excentricityInner);
    }
    else if (radius < m_FarFieldRadius)
    {
        // Transition region: reduce back to circular orbits
        const float64_t t = (radius - m_Config.radius) / (m_FarFieldRadius - m_Config.radius);
        return m_Config.excentricityOuter + t * (1.0 - m_Config.excentricityOuter);
    }
    else
    {
        return 1.0; // Far field: circular orbits
    }
}

/**************************************************************************************************/
float64_t Model::GetOrbitalVelocity(float64_t radius) const
{
    constexpr float64_t c_CentralMass = 100.0; // Solar masses

    float64_t velocityKmPerSec;

    if (m_Config.hasDarkMatter)
    {
        const float64_t totalMass =
            CalculateDiskMass(radius) + CalculateHaloMass(radius) + c_CentralMass;
        velocityKmPerSec = 20000.0 * std::sqrt(Math::c_GravitationalConstant * totalMass / radius);
    }
    else
    {
        const float64_t totalMass = CalculateDiskMass(radius) + c_CentralMass;
        velocityKmPerSec = 20000.0 * std::sqrt(Math::c_GravitationalConstant * totalMass / radius);
    }

    // Convert linear velocity (km/s) to angular velocity (degrees/year)
    const float64_t radiusKm = radius * Math::c_PcToKm;
    return Math::VelocityToAngularVelocity(velocityKmPerSec, radiusKm);
}

/**************************************************************************************************/
float64_t Model::GetAngularOffsetAt(float64_t radius) const
{
    return radius * m_Config.angularOffset;
}

/**************************************************************************************************/
float64_t Model::CalculateDiskMass(float64_t radius) const
{
    constexpr float64_t c_DiskThickness = 2000.0;
    constexpr float64_t c_DensityCenter = 1.0;
    constexpr float64_t c_ScaleLength = 2000.0;

    return c_DensityCenter * std::exp(-radius / c_ScaleLength) * (radius * radius) * Math::c_Pi *
           c_DiskThickness;
}

/**************************************************************************************************/
float64_t Model::CalculateHaloMass(float64_t radius) const
{
    constexpr float64_t c_HaloDensityCenter = 0.15;
    constexpr float64_t c_HaloScaleLength = 2500.0;

    const float64_t densityFactor = 1.0 / (1.0 + std::pow(radius / c_HaloScaleLength, 2.0));
    return c_HaloDensityCenter * densityFactor * (4.0 * Math::c_Pi * std::pow(radius, 3.0) / 3.0);
}

/**************************************************************************************************/
void Model::SingleTimeStep(float64_t timeYears)
{
    m_TimeStep = timeYears;
    m_Time += timeYears;

    // Update stars
    for (auto& star : m_Stars)
    {
        const glm::dvec2 oldPos = star.position;
        star.theta += star.velTheta * timeYears;
        star.CalculatePosition(m_Config.perturbationN, m_Config.perturbationAmp);
        star.velocity = star.position - oldPos;
    }

    // Update dust
    for (auto& dust : m_Dust)
    {
        dust.theta += dust.velTheta * timeYears;
        dust.CalculatePosition(m_Config.perturbationN, m_Config.perturbationAmp);
    }

    // Update H2 regions
    for (auto& h2 : m_H2Regions)
    {
        h2.theta += h2.velTheta * timeYears;
        h2.CalculatePosition(m_Config.perturbationN, m_Config.perturbationAmp);
    }
}

/**************************************************************************************************/
void Model::SetRadius(float64_t radius)
{
    m_Config.radius = radius;
    Reset();
}

/**************************************************************************************************/
void Model::SetCoreRadius(float64_t radius)
{
    m_Config.coreRadius = radius;
    Reset();
}

/**************************************************************************************************/
void Model::SetSigma(float64_t sigma)
{
    m_Config.sigma = sigma;
    Reset();
}

/**************************************************************************************************/
void Model::SetAngularOffset(float64_t offset)
{
    m_Config.angularOffset = offset;
    Reset();
}

/**************************************************************************************************/
void Model::SetExcentricityInner(float64_t ex)
{
    m_Config.excentricityInner = ex;
    Reset();
}

/**************************************************************************************************/
void Model::SetExcentricityOuter(float64_t ex)
{
    m_Config.excentricityOuter = ex;
    Reset();
}

/**************************************************************************************************/
void Model::SetPerturbationN(uint32_t n)
{
    m_Config.perturbationN = std::max(0U, n);
}

/**************************************************************************************************/
void Model::SetPerturbationAmp(float64_t amp)
{
    m_Config.perturbationAmp = std::max(0.0, amp);
}

/**************************************************************************************************/
void Model::SetDustRenderSize(float64_t size)
{
    m_Config.dustRenderSize = std::max(size, 1.0);
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
void Model::InitializeStars()
{
    const uint32_t numStars = m_Config.numStars;
    const uint32_t numDust = numStars / 2;
    const uint32_t numH2 = 300;

    m_Stars.clear();
    m_Stars.reserve(numStars);

    m_Dust.clear();
    m_Dust.reserve(numDust);

    m_H2Regions.clear();
    m_H2Regions.reserve(numH2 * 2);

    InitializeAlignmentStars();
    InitializeRegularStars();
    InitializeDust();
    InitializeH2Regions();
}

/**************************************************************************************************/
void Model::InitializeAlignmentStars()
{
    // First three stars for camera alignment

    // Star 0: Black hole at center
    StarData blackHole;
    blackHole.semiMajorAxis = 0.0;
    blackHole.semiMinorAxis = 0.0;
    blackHole.angle = 0.0;
    blackHole.theta = 0.0;
    blackHole.velTheta = 0.0;
    blackHole.center = {0.0, 0.0};
    blackHole.temperature = 6000.0;
    m_Stars.push_back(blackHole);

    // Star 1: Edge of core
    StarData coreEdge;
    coreEdge.semiMajorAxis = m_Config.coreRadius;
    coreEdge.semiMinorAxis = m_Config.coreRadius * GetExcentricity(m_Config.coreRadius);
    coreEdge.angle = GetAngularOffsetAt(m_Config.coreRadius);
    coreEdge.theta = 0.0;
    coreEdge.center = {0.0, 0.0};
    coreEdge.velTheta = GetOrbitalVelocity((coreEdge.semiMajorAxis + coreEdge.semiMinorAxis) / 2.0);
    coreEdge.temperature = 6000.0;
    m_Stars.push_back(coreEdge);

    // Star 2: Edge of disk
    StarData diskEdge;
    diskEdge.semiMajorAxis = m_Config.radius;
    diskEdge.semiMinorAxis = m_Config.radius * GetExcentricity(m_Config.radius);
    diskEdge.angle = GetAngularOffsetAt(m_Config.radius);
    diskEdge.theta = 0.0;
    diskEdge.center = {0.0, 0.0};
    diskEdge.velTheta = GetOrbitalVelocity((diskEdge.semiMajorAxis + diskEdge.semiMinorAxis) / 2.0);
    diskEdge.temperature = 6000.0;
    m_Stars.push_back(diskEdge);
}

/**************************************************************************************************/
void Model::InitializeRegularStars()
{
    // Setup cumulative distribution function for realistic star placement
    CumulativeDistributionFunction cdf;
    cdf.SetupRealistic(1.0,                   // Maximum intensity
                       0.02,                  // k (bulge parameter)
                       m_Config.radius / 3.0, // Disc scale length
                       m_Config.coreRadius,   // Bulge radius
                       0.0,                   // Start of intensity curve
                       m_FarFieldRadius,      // End of intensity curve
                       1000                   // Number of supporting points
    );

    std::random_device                        rd;
    std::mt19937                              gen(rd());
    std::uniform_real_distribution<float64_t> dist(0.0, 1.0);

    const float64_t binWidth = m_FarFieldRadius / 100.0;

    for (uint32_t i = 3; i < m_Config.numStars; ++i)
    {
        const float64_t radius = cdf.ValFromProb(dist(gen));

        StarData star;
        star.semiMajorAxis = radius;
        star.semiMinorAxis = radius * GetExcentricity(radius);
        star.angle = GetAngularOffsetAt(radius);
        star.theta = 360.0 * dist(gen);
        star.velTheta = GetOrbitalVelocity(radius);
        star.center = {0.0, 0.0};
        star.temperature = 6000.0 + (4000.0 * dist(gen)) - 2000.0;
        // Original values from Galaxy-Renderer - now using real particle.bmp
        star.magnitude = 0.3 + 0.2 * dist(gen); // Range: 0.3-0.5

        // Update histogram
        const uint32_t bin = static_cast<uint32_t>(
            std::min((star.semiMajorAxis + star.semiMinorAxis) / (2.0 * binWidth), 99.0));
        m_RadialHistogram[bin]++;

        m_Stars.push_back(star);
    }
}

/**************************************************************************************************/
void Model::InitializeDust()
{
    CumulativeDistributionFunction cdf;
    cdf.SetupRealistic(1.0, 0.02, m_Config.radius / 3.0, m_Config.coreRadius, 0.0, m_FarFieldRadius,
                       1000);

    std::random_device                        rd;
    std::mt19937                              gen(rd());
    std::uniform_real_distribution<float64_t> dist(0.0, 1.0);

    const int32_t   numDust = m_Config.numStars / 2;
    const float64_t binWidth = m_FarFieldRadius / 100.0;

    for (int32_t i = 0; i < numDust; ++i)
    {
        float64_t radius;

        if (i % 4 == 0)
        {
            radius = cdf.ValFromProb(dist(gen));
        }
        else
        {
            const float64_t x = 2.0 * m_Config.radius * dist(gen) - m_Config.radius;
            const float64_t y = 2.0 * m_Config.radius * dist(gen) - m_Config.radius;
            radius = std::sqrt(x * x + y * y);
        }

        StarData dust;
        dust.semiMajorAxis = radius;
        dust.semiMinorAxis = radius * GetExcentricity(radius);
        dust.angle = GetAngularOffsetAt(radius);
        dust.theta = 360.0 * dist(gen);
        dust.velTheta = GetOrbitalVelocity((dust.semiMajorAxis + dust.semiMinorAxis) / 2.0);
        dust.center = {0.0, 0.0};
        dust.temperature = 5000.0 + radius / 4.5; // Temperature gradient
        // Original values from Galaxy-Renderer - now using real particle.bmp
        dust.magnitude = 0.015 + 0.01 * dist(gen); // Range: 0.015-0.025

        const uint32_t bin = static_cast<uint32_t>(
            std::min((dust.semiMajorAxis + dust.semiMinorAxis) / (2.0 * binWidth), 99.0));
        m_RadialHistogram[bin]++;

        m_Dust.push_back(dust);
    }
}

/**************************************************************************************************/
void Model::InitializeH2Regions()
{
    std::random_device                        rd;
    std::mt19937                              gen(rd());
    std::uniform_real_distribution<float64_t> dist(0.0, 1.0);

    const int32_t       numH2Pairs = 300;
    const float64_t     binWidth = m_FarFieldRadius / 100.0;
    constexpr float64_t c_PairDistance = 1000.0; // 1000 parsecs between pair points

    for (int32_t i = 0; i < numH2Pairs; ++i)
    {
        const float64_t x = 2.0 * m_Config.radius * dist(gen) - m_Config.radius;
        const float64_t y = 2.0 * m_Config.radius * dist(gen) - m_Config.radius;
        const float64_t radius = std::sqrt(x * x + y * y);

        // First point of pair
        StarData h2_1;
        h2_1.semiMajorAxis = radius;
        h2_1.semiMinorAxis = radius * GetExcentricity(radius);
        h2_1.angle = GetAngularOffsetAt(radius);
        h2_1.theta = 360.0 * dist(gen);
        h2_1.velTheta = GetOrbitalVelocity((h2_1.semiMajorAxis + h2_1.semiMinorAxis) / 2.0);
        h2_1.center = {0.0, 0.0};
        h2_1.temperature = 6000.0 + (6000.0 * dist(gen)) - 3000.0;
        // Original values from Galaxy-Renderer - now using real particle.bmp
        h2_1.magnitude = 0.1 + 0.05 * dist(gen); // Range: 0.1-0.15

        // Second point 1000 pc away
        StarData h2_2 = h2_1;
        h2_2.semiMajorAxis = radius + c_PairDistance;
        h2_2.semiMinorAxis = radius * GetExcentricity(radius);

        const uint32_t bin = static_cast<uint32_t>(
            std::min((h2_1.semiMajorAxis + h2_1.semiMinorAxis) / (2.0 * binWidth), 99.0));
        m_RadialHistogram[bin] += 2;

        m_H2Regions.push_back(h2_1);
        m_H2Regions.push_back(h2_2);
    }
}

} // namespace Orogena::Galaxy
