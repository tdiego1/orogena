/**************************************************************************************************/
/**
 * @file galaxy_model.h
 * @brief Spiral galaxy simulation model
 *
 * @details
 * Encapsulates the geometric and physical properties of a spiral galaxy, including star
 * distribution, orbital mechanics, and dark matter effects.
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

#include <memory>
#include <vector>

#include "galaxy_types.h"

#include <array>

namespace Orogena::Galaxy
{

/**
 * @brief Complete spiral galaxy simulation
 *
 * @details
 * Generates and simulates a spiral galaxy with realistic orbital mechanics,
 * star distribution, and optional dark matter. Supports dust clouds and
 * H2 regions for enhanced visual realism.
 *
 * Example usage:
 * @code
 * GalaxyConfig config;
 * config.numStars = 50000;
 * config.hasDarkMatter = true;
 *
 * Model galaxy{config};
 * galaxy.SingleTimeStep(1.0); // Advance 1 year
 * @endcode
 */
class Model
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Contruct galaxy with default parameters
     */
    Model();

    /**
     * @brief Construct galaxy with custom configuration
     *
     * @param config Galaxy generation parameters
     */
    explicit Model(const GalaxyConfig& config);

    ~Model() = default;

    // Delete copy operations
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    // Default move operations
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    //=============================================================================================
    // Simulation Control
    //=============================================================================================

    /**
     * @brief Reset galaxy with new configuration
     * @param config New galaxy parameters
     */
    void Reset(const GalaxyConfig& config);

    /**
     * @brief Reset galaxy with current configuration
     */
    void Reset();

    /**
     * @brief Advance simulation by time step
     * @param timeYears Time step in years
     */
    void SingleTimeStep(float64_t timeYears);

    /**
     * @brief Toggle dark matter physics on/off
     */
    void ToggleDarkMatter();

    //=============================================================================================
    // Data Access
    //=============================================================================================

    /**
     * @brief Get star data array
     * @return Pointer to star array (size = GetNumStars())
     */
    const std::vector<StarData>& GetStars() const
    {
        return m_Stars;
    }

    /**
     * @brief Get dust particle data
     * @return Pointer to dust array (size = GetNumDust())
     */
    const std::vector<StarData>& GetDust() const
    {
        return m_Dust;
    }

    /**
     * @brief Get H2 region data
     * @return Pointer to H2 array (size = GetNumH2() * 2)
     */
    const std::vector<StarData>& GetH2Regions() const
    {
        return m_H2Regions;
    }

    /**
     * @brief Get star position by index
     * @param index Star index
     * @return Star position in parsecs
     * @throws std::out_of_range if index >= GetNumStars()
     */
    const glm::dvec2& GetStarPosition(uint32_t index) const;

    //=============================================================================================
    // Properties
    //=============================================================================================

    float64_t GetRadius() const
    {
        return m_Config.radius;
    }
    float64_t GetCoreRadius() const
    {
        return m_Config.coreRadius;
    }
    float64_t GetFarFieldRadius() const
    {
        return m_FarFieldRadius;
    }
    float64_t GetSigma() const
    {
        return m_Config.sigma;
    }
    float64_t GetDustRenderSize() const
    {
        return m_Config.dustRenderSize;
    }
    float64_t GetAngularOffset() const
    {
        return m_Config.angularOffset;
    }
    float64_t GetExcentricityInner() const
    {
        return m_Config.excentricityInner;
    }
    float64_t GetExcentricityOuter() const
    {
        return m_Config.excentricityOuter;
    }
    float64_t GetTimeStep() const
    {
        return m_TimeStep;
    }
    float64_t GetTime() const
    {
        return m_Time;
    }

    uint32_t GetNumStars() const
    {
        return static_cast<uint32_t>(m_Stars.size());
    }
    uint32_t GetNumDust() const
    {
        return static_cast<uint32_t>(m_Dust.size());
    }
    uint32_t GetNumH2() const
    {
        return static_cast<uint32_t>(m_H2Regions.size() / 2U);
    }
    uint32_t GetPerturbationN() const
    {
        return m_Config.perturbationN;
    }
    float64_t GetPerturbationAmp() const
    {
        return m_Config.perturbationAmp;
    }

    /**
     * @brief Get star count histogram by radius
     * @return Array of 100 bins showing star distribution
     */
    const std::array<int32_t, 100>& GetRadialHistogram() const
    {
        return m_RadialHistogram;
    }

    //=============================================================================================
    // Orbital Properties (radius-dependent)
    //=============================================================================================

    /**
     * @brief Calculate orbit excentricity at given radius
     * @param radius Distance from galactic center (parsecs)
     * @return Excentricity value
     */
    float64_t GetExcentricity(float64_t radius) const;

    /**
     * @brief Calculate orbital velocity at given radius
     * @param radius Distance from galactic center (parsecs)
     * @return Angular velocity (degrees/year)
     * @note Includes dark matter effects if enabled
     */
    float64_t GetOrbitalVelocity(float64_t radius) const;

    /**
     * @brief Calculate angular offset for spiral arms
     * @param radius Distance from galactic center (parsecs)
     * @return Angular offset (radians)
     */
    float64_t GetAngularOffsetAt(float64_t radius) const;

    //=============================================================================================
    // Configuration Setters
    //=============================================================================================

    void SetRadius(float64_t radius);
    void SetCoreRadius(float64_t radius);
    void SetSigma(float64_t sigma);
    void SetAngularOffset(float64_t offset);
    void SetExcentricityInner(float64_t ex);
    void SetExcentricityOuter(float64_t ex);
    void SetPerturbationN(uint32_t n);
    void SetPerturbationAmp(float64_t amp);
    void SetDustRenderSize(float64_t size);

  private:
    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Initialize star data arrays
     */
    void InitializeStars();

    /**
     * @brief Initialize alignment stars in core region
     */
    void InitializeAlignmentStars();

    /**
     * @brief Initialize regular stars in disk region
     */
    void InitializeRegularStars();

    /**
     * @brief Initialize dust particle data
     */
    void InitializeDust();

    /**
     * @brief Initialize H2 region data
     */
    void InitializeH2Regions();

    /**
     * @brief Calculate mass of visible disk at given radius
     */
    float64_t CalculateDiskMass(float64_t radius) const;

    /**
     * @brief Calculate mass of dark matter halo at given radius
     */
    float64_t CalculateHaloMass(float64_t radius) const;

    //=============================================================================================
    // Private Members
    //=============================================================================================

    GalaxyConfig m_Config;

    std::vector<StarData> m_Stars;
    std::vector<StarData> m_Dust;
    std::vector<StarData> m_H2Regions;

    std::array<int32_t, 100> m_RadialHistogram{};

    float64_t m_FarFieldRadius{0.0}; ///< Radius where density waves become circular
    float64_t m_Time{0.0};           ///< Current simulation time (years)
    float64_t m_TimeStep{0.0};       ///< Last time step size (years)
};

} // namespace Orogena::Galaxy
