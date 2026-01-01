/**************************************************************************************************/
/**
 * @file stellar_star.cpp
 * @brief Implementation of Star
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "stellar_star.h"

#include <cmath>

namespace Orogena::Stellar
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

Star::Star() : m_MassMsol(1.0F), m_CurrentAgeGyr(4.5F)
{
    RecalculateProperties();
}

Star::~Star()
{
    // Destructor body
}

//=================================================================================================
// Public Functions
//=================================================================================================

void Star::RecalculateProperties()
{
    // Calculate luminosity using mass-luminosity relation
    m_LuminosityLsol = CalculateLuminosityFromMass(m_MassMsol);

    // Calculate radius using mass-radius relation (approximate)
    m_RadiusRsol = CalculateRadiusFromMass(m_MassMsol);
}

//=================================================================================================
// Private Functions
//=================================================================================================

float32_t Star::CalculateLuminosityFromMass(float32_t massMsol)
{
    // Mass thresholds
    static constexpr float32_t c_LowMassStarMsol = 0.43F; // Msol
    static constexpr float32_t c_MidMassStarMsol = 2.0F;  // Msol

    // Power law exponents
    static constexpr float32_t c_LowMassExponent = 2.3F;  // M < 0.43 Msol
    static constexpr float32_t c_MidMassExponent = 4.0F;  // 0.43 <= M < 2 Msol
    static constexpr float32_t c_HighMassExponent = 3.5F; // M >= 2 Msol

    // Coefficients
    static constexpr float32_t c_LowMassCoefficient = 0.23F; // Low-mass stars
    static constexpr float32_t c_HighMassCoefficient = 1.4F; // High-mass stars

    // Calculate luminosity using mass-luminosity relation
    // Different power laws fffor different mass ranges
    if (massMsol < c_LowMassStarMsol)
    {
        // Low-mass stars (M < 0.43 Msol): L ∝ M^2.3
        return c_LowMassCoefficient * std::pow(massMsol, c_LowMassExponent);
    }
    else if (massMsol < c_MidMassStarMsol)
    {
        // Mid-mass stars (0.43 <= M < 2 Msol): L ∝ M^4
        return std::pow(massMsol, c_MidMassExponent);
    }
    else
    {
        // High-mass stars (M >= 2 Msol): L ∝ M^3.5
        return c_HighMassCoefficient * std::pow(massMsol, c_HighMassExponent);
    }
}

float32_t Star::CalculateRadiusFromMass(float32_t massMsol)
{
    // Mass-radius relation for main sequence stars
    // Based on Demircan & Kahraman (1991)
    // Diffferent power laws for different mass ranges

    // Mass thresholds
    static constexpr float32_t c_SolarMassThreshold = 1.0F; // Msol
    static constexpr float32_t c_HighMassThreshold = 1.66F; // Msol

    // Power law exponents
    static constexpr float32_t c_LowMassRadiusExponent = 0.8F;    // M < 1 Msol
    static constexpr float32_t c_MidMassRadiusExponent = 0.57F;   // 1 <= M < 1.66 Msol
    static constexpr float32_t c_HighMassRadiusExponent = 0.555F; // M >= 1.66 Msol

    // Coefficient for high-mass stars
    static constexpr float32_t c_HighMassRadiusCoefficient = 1.33F;

    if (massMsol < c_SolarMassThreshold)
    {
        // Low-mass stars (M < 1 Msol): R ∝ M^0.8
        // Applies to red dwarfs (M-type) and orange dwarfs (K-type)
        return std::pow(massMsol, c_LowMassRadiusExponent);
    }
    else if (massMsol < c_HighMassThreshold)
    {
        // Solar-mass stars (1 <= M < 1.66 Msol): R ∝ M^0.57
        // Applies to yellow stars (G-type) and some white stars (F-type)
        return std::pow(massMsol, c_MidMassRadiusExponent);
    }
    else
    {
        // High-mass stars (M >= 1.66 Msol): Modified power law
        // Applies to hot stars (A, B, O types)
        // Coefficient accounts for different internal structure
        return c_HighMassRadiusCoefficient * std::pow(massMsol, c_HighMassRadiusExponent);
    }
}

float32_t Star::CalculateMaximumAge(float32_t massMsol, float32_t luminosityLsol)
{
    // Main sequence lifetime using mass and luminosity
    // Lifetime = (Fuel Supply / Burn Rate) x Efficiency Factor

    // Solar lifetime baseline (Gyr)
    static constexpr float32_t c_SolarLifetimeGyr = 10.0F;

    // Mass thresholds for efficiency correction
    static constexpr float32_t c_LowMassThreshold = 0.5F;
    static constexpr float32_t c_HighMassThreshold = 2.0F;

    // Core hydrogen fraction (fuel available for fusion)
    // Lower-mass stars use proportionally less of their mass for fusion
    float32_t hydrogenFraction;

    if (massMsol < c_LowMassThreshold)
    {
        // Red dwarfs: Fully convective, can use ~100% of their hydrogen
        hydrogenFraction = 1.0F;
    }
    else if (massMsol < c_HighMassThreshold)
    {
        // Solar-type stars: ~10% of mass participates in fusion
        hydrogenFraction = 0.1F;
    }
    else
    {
        // Massive stars: ~7% of mass in fusion zone
        hydrogenFraction = 0.07F;
    }

    // Calculate lifetime: (Mass x Fuel Fraction / Luminosity) x Solar Constant
    // The formula naturally gives ~10 Gyr for the Sun (M=1, L=1, fraction=0.1)
    float32_t lifetimeGyr = c_SolarLifetimeGyr * (massMsol * hydrogenFraction) / luminosityLsol;

    return lifetimeGyr;
}

} // namespace Orogena::Stellar
