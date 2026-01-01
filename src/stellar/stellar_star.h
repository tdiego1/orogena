/**************************************************************************************************/
/**
 * @file stellar_star.h
 * @brief Brief description
 *
 * @details
 * Detailed description of the class purpose and responsibilities.
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <string>

#include "utils/utils_types.h"

namespace Orogena::Stellar
{

/**
 * @brief Brief description of the class
 *
 * @details
 * Detailed explanation of what this class does and its responsibilities.
 */
class Star
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    static constexpr float32_t c_MinimumMassMsol = 0.075F; ///< Minimum stellar mass (Msol)
    static constexpr float32_t c_MaximumMassMsol = 100.0F; ///< Maximum stellar mass (Msol)

    enum class SpectralType
    {
        O, ///< Hot, blue stars
        B, ///< Blue-white stars
        A, ///< White stars
        F, ///< Yellow-white stars
        G, ///< Yellow stars (like the Sun)
        K, ///< Orange stars
        M  ///< Red, cool stars
    };

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    Star();
    ~Star();

    // Delete copy operations
    Star(const Star&) = delete;
    Star& operator=(const Star&) = delete;

    // Default move operations
    Star(Star&&) = default;
    Star& operator=(Star&&) = default;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Recalculate derived star properties
     */
    void RecalculateProperties();

    //=============================================================================================
    // Setters/Getters
    //=============================================================================================

    /**
     * @brief Set star mass
     *
     * @param massMsol Mass in solar masses (Msol)
     */
    void SetMass(float32_t massMsol)
    {
        m_MassMsol = massMsol;
    }

    /**
     * @brief Set current star age
     *
     * @param ageGyr Age in Giga-years (Gyr)
     */
    void SetCurrentAge(float32_t ageGyr)
    {
        m_CurrentAgeGyr = ageGyr;
    }

    /**
     * @brief Get star mass
     *
     * @return float32_t Mass in solar masses (Msol)
     */
    float32_t GetMass() const
    {
        return m_MassMsol;
    }

    /**
     * @brief Get current star age
     *
     * @return float32_t Age in Giga-years (Gyr)
     */
    float32_t GetCurrentAge() const
    {
        return m_CurrentAgeGyr;
    }

    /**
     * @brief Get star spectral type
     *
     * @return SpectralType Spectral classification
     */
    SpectralType GetSpectralType() const
    {
        return m_SpectralType;
    }

    /**
     * @brief Get star spectral subclass
     *
     * @return float32_t Spectral subclass (0.0 to 9.0)
     */
    float32_t GetSpectralSubClass() const
    {
        return m_SpectralSubClass;
    }

    /**
     * @brief Get spectral classification as string
     *
     * @return std::string Spectral type as string (e.g., "G2.8")
     */
    std::string GetSpectralClassString() const;

    /**
     * @brief Get bool indicating if star is on main sequence.
     *
     * @return true If star is on main sequence, false otherwise.
     */
    bool IsMainSequence() const
    {
        return m_IsMainSequence;
    }

    /**
     * @brief Get star maximum main sequence age
     *
     * @return float32_t Maximum age in Giga-years (Gyr)
     */
    float32_t GetMaximumAge() const
    {
        return m_MaxiumAgeGyr;
    }

    /**
     * @brief Get star radius
     *
     * @return float32_t Radius in solar radii (Rsol)
     */
    float32_t GetRadius() const
    {
        return m_RadiusRsol;
    }

    /**
     * @brief Get star luminosity
     *
     * @return float32_t Luminosity in solar luminosities (Lsol)
     */
    float32_t GetLuminosity() const
    {
        return m_LuminosityLsol;
    }

    /**
     * @brief Get star density
     *
     * @return float32_t Density in solar densities (Dsol)
     */
    float32_t GetDensity() const
    {
        return m_DensityDsol;
    }

    /**
     * @brief Get star effective temperature
     *
     * @return uint32_t Temperature in Kelvin (K)
     */
    uint32_t GetTemperature() const
    {
        return m_TemperatureK;
    }

    /**
     * @brief Get star color as RGB
     *
     * @return Utils::ColorRGBF Star color (0.0-1.0)
     */
    Utils::ColorRGBF GetColorRGB() const
    {
        return m_ColorRGB;
    }

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

    //=============================================================================================
    // Private Members
    //=============================================================================================

    // Input parameters
    float32_t m_MassMsol;      ///< Mass in solar masses (Our sun = 1.0 Msol = 1.989 x 10^30 kg)
    float32_t m_CurrentAgeGyr; ///< Current age in Giga-years (1 Gyr = 1 billion earth years)

    // Derived parameters
    SpectralType     m_SpectralType;     ///< Spectral classification of the star
    float32_t        m_SpectralSubClass; ///< Spectral subclass (0.0 to 9.0)
    bool             m_IsMainSequence;   ///< Whether star is on main sequence
    float32_t        m_MaxiumAgeGyr;     ///< Main sequence lifetime (1 Gyr = 1 billion earth years)
    float32_t        m_RadiusRsol;     ///< Radius in solar radii (Our sun = 1.0 Rsol = 695,700 km)
    float32_t        m_LuminosityLsol; ///< Solar luminosity (Our sun = 1.0 Lsol = 3.828 x 10^26 W)
    float32_t        m_DensityDsol;    ///< Star density in Dsol (Our sun = 1.0 Dsol = 1.408 g/cm^3)
    uint32_t         m_TemperatureK;   ///< Effective temperature in Kelvin
    Utils::ColorRGBF m_ColorRGB;       ///< Star color as RGB (0.0-1.0), derived from temperature
};

} // namespace Orogena::Stellar
