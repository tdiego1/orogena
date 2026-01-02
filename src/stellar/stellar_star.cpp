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

#include "utils/utils_logger.h"
#include "utils/utils_types.h"

#include <cmath>

namespace Orogena::Stellar
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

// Blackbody color table based on Mitchell Charity's work
// Source: http://www.vendian.org/mncharity/dir3/blackbody/
//
// These values are computed from Planck's law and CIE color matching functions,
// converted to sRGB color space with D58 white point.
//
// Temperature (K) -> RGB (0-1 scale, sRGB)
static constexpr struct ColorEntry
{
    float32_t        temperatureK;
    Utils::ColorRGBF color;
} c_ColorTable[] = {
    // Red dwarfs (M-type)
    {1000.0F, {1.0000F, 0.0402F, 0.0000F}},
    {1500.0F, {1.0000F, 0.1801F, 0.0000F}},
    {2000.0F, {1.0000F, 0.2996F, 0.0041F}},
    {2500.0F, {1.0000F, 0.4317F, 0.0735F}},
    {3000.0F, {1.0000F, 0.5512F, 0.1765F}},
    {3500.0F, {1.0000F, 0.6575F, 0.3042F}},

    // Orange/red dwarfs (K-type)
    {4000.0F, {1.0000F, 0.7512F, 0.4481F}},
    {4500.0F, {1.0000F, 0.8331F, 0.6005F}},

    // Yellow stars (G-type, Sun-like)
    {5000.0F, {1.0000F, 0.9046F, 0.7559F}},
    {5500.0F, {1.0000F, 0.9668F, 0.9098F}},
    {5772.0F, {1.0000F, 0.9180F, 0.8400F}}, // Sun (exact)
    {6000.0F, {0.9438F, 0.9636F, 1.0000F}},
    {6500.0F, {0.8313F, 0.8878F, 1.0000F}},

    // White stars (F-type)
    {7000.0F, {0.7467F, 0.8283F, 1.0000F}},
    {7500.0F, {0.6814F, 0.7804F, 1.0000F}},

    // Blue-white stars (A-type)
    {8000.0F, {0.6297F, 0.7412F, 1.0000F}},
    {8500.0F, {0.5880F, 0.7085F, 1.0000F}},
    {9000.0F, {0.5538F, 0.6809F, 1.0000F}},
    {9500.0F, {0.5253F, 0.6574F, 1.0000F}},
    {10000.0F, {0.5013F, 0.6372F, 1.0000F}},

    // Blue stars (B-type)
    {10500.0F, {0.4808F, 0.6196F, 1.0000F}},
    {11000.0F, {0.4631F, 0.6041F, 1.0000F}},
    {11500.0F, {0.4478F, 0.5905F, 1.0000F}},
    {12000.0F, {0.4344F, 0.5784F, 1.0000F}},
    {12500.0F, {0.4226F, 0.5676F, 1.0000F}},
    {13000.0F, {0.4120F, 0.5578F, 1.0000F}},
    {13500.0F, {0.4027F, 0.5491F, 1.0000F}},
    {14000.0F, {0.3943F, 0.5411F, 1.0000F}},
    {14500.0F, {0.3867F, 0.5339F, 1.0000F}},
    {15000.0F, {0.3798F, 0.5273F, 1.0000F}},

    // Hot blue stars (O-type)
    {16000.0F, {0.3678F, 0.5156F, 1.0000F}},
    {17000.0F, {0.3578F, 0.5057F, 1.0000F}},
    {18000.0F, {0.3493F, 0.4972F, 1.0000F}},
    {19000.0F, {0.3419F, 0.4898F, 1.0000F}},
    {20000.0F, {0.3356F, 0.4833F, 1.0000F}},

    {22000.0F, {0.3251F, 0.4724F, 1.0000F}},
    {24000.0F, {0.3168F, 0.4638F, 1.0000F}},
    {26000.0F, {0.3101F, 0.4567F, 1.0000F}},
    {28000.0F, {0.3046F, 0.4509F, 1.0000F}},
    {30000.0F, {0.3000F, 0.4459F, 1.0000F}},

    {35000.0F, {0.2912F, 0.4364F, 1.0000F}},
    {40000.0F, {0.2850F, 0.4296F, 1.0000F}}};

static constexpr int32_t c_ColorTableSize = sizeof(c_ColorTable) / sizeof(c_ColorTable[0]);

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

    // Calculate density from mass and radius
    m_DensityDsol = CalculateDensity(m_MassMsol, m_RadiusRsol);

    // Calculate maximum main sequence lifetime
    m_MaximumAgeGyr = CalculateMaximumAge(m_MassMsol, m_LuminosityLsol);

    // Determine if star is still on main sequence
    m_IsMainSequence = (m_CurrentAgeGyr < m_MaximumAgeGyr);
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

float32_t Star::CalculateDensity(float32_t massMsol, float32_t radiusRsol)
{
    // Density calculation: Density = M / R^3 in solar units

    // Constants for validation
    static constexpr float32_t c_MinRadiusRsol = 0.001F;
    static constexpr float32_t c_MaxDensityDsol = 10000.0F;

    // Validate input radius
    if (radiusRsol < c_MinRadiusRsol)
    {
        Log::Warn("Star radius too small ({} Rsol), clamping to minimum", radiusRsol);
        radiusRsol = c_MinRadiusRsol;
    }

    // Calculate R^3
    const float32_t radius_cubed = radiusRsol * radiusRsol * radiusRsol;

    // Density in solar units
    const float32_t density_dsol = massMsol / radius_cubed;

    // Sanity check: clamp to resonable maximum
    if (density_dsol > c_MaxDensityDsol)
    {
        Log::Warn("Calculated density {} Dsol exceeds maximum, clamping", density_dsol);
        return c_MaxDensityDsol;
    }

    return density_dsol;
}

float32_t Star::CalculateTemperature(float32_t luminosityLsol, float32_t radiusRsol)
{
    // Calculate effective temperature using Stefan-Boltzmann law
    // T = Tsol * (L/R^2)^0.25
    // Where Tsol = 5776 K (Sun's effective temperature)

    // Constants
    static constexpr float32_t c_solar_temp_k = 5776.0F; // Kelvin
    static constexpr float32_t c_min_radius_rsol = 0.001F;
    static constexpr float32_t c_stefan_boltzmann_exponent = 0.25F; // 1/4 power

    // Validate input radius
    if (radiusRsol < c_min_radius_rsol)
    {
        Log::Warn("Star radius too small ({} Rsol) for temperature calculation, clamping",
                  radiusRsol);
        radiusRsol = c_min_radius_rsol;
    }

    // Calculate R^2
    const float32_t c_radius_squared = radiusRsol * radiusRsol;

    // Calculate luminosity-to-area ratio
    const float32_t c_lum_per_area = luminosityLsol / c_radius_squared;

    // Apply Stefan-Boltzmann law: T = Tsol * (L/A)^0.25
    const float32_t c_temperature_k =
        c_solar_temp_k * std::pow(c_lum_per_area, c_stefan_boltzmann_exponent);

    return c_temperature_k;
}

Utils::ColorRGBF Star::CalculateColorFromTemperature(float32_t temperatureK)
{
    // Handle edge cases
    if (temperatureK <= c_ColorTable[0].temperatureK)
    {
        return c_ColorTable[0].color;
    }

    if (temperatureK >= c_ColorTable[c_ColorTableSize - 1].temperatureK)
    {
        return c_ColorTable[c_ColorTableSize - 1].color;
    }

    // Find bracketing entries for linear interpolation
    for (int32_t i = 0; i < c_ColorTableSize - 1; ++i)
    {
        if (temperatureK >= c_ColorTable[i].temperatureK &&
            temperatureK <= c_ColorTable[i + 1].temperatureK)
        {
            // Linear interpolation
            const float32_t t = (temperatureK - c_ColorTable[i].temperatureK) /
                                (c_ColorTable[i + 1].temperatureK - c_ColorTable[i].temperatureK);

            Utils::ColorRGBF color;
            color.r = c_ColorTable[i].color.r +
                      t * (c_ColorTable[i + 1].color.r - c_ColorTable[i].color.r);
            color.g = c_ColorTable[i].color.g +
                      t * (c_ColorTable[i + 1].color.g - c_ColorTable[i].color.g);
            color.b = c_ColorTable[i].color.b +
                      t * (c_ColorTable[i + 1].color.b - c_ColorTable[i].color.b);

            return color;
        }
    }
    // Should never reach here
    return c_ColorTable[c_ColorTableSize / 2].color;
}

} // namespace Orogena::Stellar
