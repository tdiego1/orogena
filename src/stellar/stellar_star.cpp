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

static constexpr float32_t c_LowMassStarMsol = 0.43F; ///< Mass threshold for low-mass stars (Msol)
static constexpr float32_t c_MidMassStarMsol = 2.0F;  ///< Mass threshold for mid-mass stars (Msol)
static constexpr float32_t c_LowMassExponent = 2.3F;  ///< Exponent for low-mass stars
static constexpr float32_t c_MidMassExponent = 4.0F;  ///< Exponent for mid-mass stars
static constexpr float32_t c_HighMassExponent = 3.5F; ///< Exponent for high-mass stars
static constexpr float32_t c_LowMassCoefficient = 0.23F; ///< Coefficient for low-mass stars
static constexpr float32_t c_HighMassCoefficient = 1.4F; ///< Coefficient for high-mass stars

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
}

//=================================================================================================
// Private Functions
//=================================================================================================

float32_t Star::CalculateLuminosityFromMass(float32_t massMsol)
{
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

} // namespace Orogena::Stellar
