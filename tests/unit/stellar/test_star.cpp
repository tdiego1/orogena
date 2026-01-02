/**************************************************************************************************/
/**
 * @file test_stellar_star.cpp
 * @brief Unit tests for Star class
 *
 * @author Diego Torres
 * @date 2026
 * @copyright Copyright (C) 2026 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "stellar/stellar_star.h"

#include <cmath>
#include <gtest/gtest.h>

namespace Orogena::Stellar
{

//=================================================================================================
// Test Fixture
//=================================================================================================

class StarTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Default star (solar mass)
        m_star = std::make_unique<Star>();
    }

    void TearDown() override
    {
        m_star.reset();
    }

    std::unique_ptr<Star> m_star;
};

//=================================================================================================
// Constructor Tests
//=================================================================================================

TEST_F(StarTest, DefaultConstructorCreatesSunLikeStar)
{
    // Default constructor should create a Sun-like star
    EXPECT_FLOAT_EQ(m_star->GetMass(), 1.0F);
    EXPECT_FLOAT_EQ(m_star->GetCurrentAge(), 4.5F);
    EXPECT_TRUE(m_star->IsMainSequence());
}

//=================================================================================================
// Mass-Luminosity Relation Tests
//=================================================================================================

TEST_F(StarTest, SolarMassProducesSolarLuminosity)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    // Solar mass should produce approximately solar luminosity
    EXPECT_NEAR(m_star->GetLuminosity(), 1.0F, 0.1F);
}

TEST_F(StarTest, LowMassStarLuminosity)
{
    // Red dwarf (0.2 Msol)
    m_star->SetMass(0.2F);
    m_star->RecalculateProperties();

    // Low-mass stars have much lower luminosity (L ∝ M^2.3)
    float32_t expected_luminosity = 0.23F * std::pow(0.2F, 2.3F);
    EXPECT_NEAR(m_star->GetLuminosity(), expected_luminosity, 0.001F);
}

TEST_F(StarTest, HighMassStarLuminosity)
{
    // Massive star (3.0 Msol)
    m_star->SetMass(3.0F);
    m_star->RecalculateProperties();

    // High-mass stars are very luminous (L ∝ M^3.5)
    float32_t expected_luminosity = 1.4F * std::pow(3.0F, 3.5F);
    EXPECT_NEAR(m_star->GetLuminosity(), expected_luminosity, 0.5F);
}

//=================================================================================================
// Mass-Radius Relation Tests
//=================================================================================================

TEST_F(StarTest, SolarMassProducesSolarRadius)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    // Solar mass should produce approximately solar radius
    EXPECT_NEAR(m_star->GetRadius(), 1.0F, 0.1F);
}

TEST_F(StarTest, RedDwarfRadius)
{
    // Red dwarf (0.5 Msol)
    m_star->SetMass(0.5F);
    m_star->RecalculateProperties();

    // Red dwarfs are smaller (R ∝ M^0.8)
    float32_t expected_radius = std::pow(0.5F, 0.8F);
    EXPECT_NEAR(m_star->GetRadius(), expected_radius, 0.01F);
}

TEST_F(StarTest, MassiveStarRadius)
{
    // Massive star (5.0 Msol)
    m_star->SetMass(5.0F);
    m_star->RecalculateProperties();

    // Massive stars are larger
    EXPECT_GT(m_star->GetRadius(), 2.0F);
}

//=================================================================================================
// Temperature Tests
//=================================================================================================

TEST_F(StarTest, SolarTemperature)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    // Sun's effective temperature is ~5776 K
    EXPECT_NEAR(m_star->GetTemperature(), 5776.0F, 200.0F);
}

TEST_F(StarTest, RedDwarfCoolerTemperature)
{
    m_star->SetMass(0.3F);
    m_star->RecalculateProperties();

    // Red dwarfs are cool (< 3700 K)
    EXPECT_LT(m_star->GetTemperature(), 3700.0F);
}

TEST_F(StarTest, BlueStarHotterTemperature)
{
    m_star->SetMass(10.0F);
    m_star->RecalculateProperties();

    // Massive blue stars are hot (> 10000 K)
    EXPECT_GT(m_star->GetTemperature(), 10000.0F);
}

//=================================================================================================
// Spectral Classification Tests
//=================================================================================================

TEST_F(StarTest, SunIsGTypeSpectral)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    EXPECT_EQ(m_star->GetSpectralType(), Star::SpectralType::G);
    EXPECT_EQ(m_star->GetSpectralTypeString(), "G");
    // Sun is G2
    EXPECT_NEAR(m_star->GetSpectralSubClass(), 2.0F, 1.0F);
}

TEST_F(StarTest, RedDwarfIsMType)
{
    m_star->SetMass(0.2F);
    m_star->RecalculateProperties();

    EXPECT_EQ(m_star->GetSpectralType(), Star::SpectralType::M);
    EXPECT_EQ(m_star->GetSpectralTypeString(), "M");
}

TEST_F(StarTest, BlueStarIsOOrBType)
{
    m_star->SetMass(15.0F);
    m_star->RecalculateProperties();

    // Massive stars are O or B type
    Star::SpectralType type = m_star->GetSpectralType();
    EXPECT_TRUE(type == Star::SpectralType::O || type == Star::SpectralType::B);
}

TEST_F(StarTest, SpectralSubClassInValidRange)
{
    // Test various masses
    for (float32_t mass = 0.1F; mass <= 20.0F; mass += 1.0F)
    {
        m_star->SetMass(mass);
        m_star->RecalculateProperties();

        // Subclass should be in range [0, 9]
        EXPECT_GE(m_star->GetSpectralSubClass(), 0.0F);
        EXPECT_LE(m_star->GetSpectralSubClass(), 9.0F);
    }
}

//=================================================================================================
// Density Tests
//=================================================================================================

TEST_F(StarTest, SolarDensity)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    // Solar density should be approximately 1.0 in solar units
    EXPECT_NEAR(m_star->GetDensity(), 1.0F, 0.2F);
}

TEST_F(StarTest, RedDwarfHigherDensity)
{
    m_star->SetMass(0.3F);
    m_star->RecalculateProperties();

    // Red dwarfs are denser (smaller radius for mass)
    EXPECT_GT(m_star->GetDensity(), 1.0F);
}

TEST_F(StarTest, MassiveStarLowerDensity)
{
    m_star->SetMass(10.0F);
    m_star->RecalculateProperties();

    // Massive stars are less dense (larger radius for mass)
    EXPECT_LT(m_star->GetDensity(), 1.0F);
}

//=================================================================================================
// Lifetime Tests
//=================================================================================================

TEST_F(StarTest, SolarLifetimeApproximately10Gyr)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    // Sun's main sequence lifetime is ~10 Gyr
    EXPECT_NEAR(m_star->GetMaximumAge(), 10.0F, 2.0F);
}

TEST_F(StarTest, RedDwarfLongLifetime)
{
    m_star->SetMass(0.3F);
    m_star->RecalculateProperties();

    // Red dwarfs live much longer than the Sun
    EXPECT_GT(m_star->GetMaximumAge(), 50.0F);
}

TEST_F(StarTest, MassiveStarShortLifetime)
{
    m_star->SetMass(10.0F);
    m_star->RecalculateProperties();

    // Massive stars burn out quickly
    EXPECT_LT(m_star->GetMaximumAge(), 0.1F);
}

TEST_F(StarTest, MainSequenceStatusCorrect)
{
    m_star->SetMass(1.0F);

    // Young star is on main sequence
    m_star->SetCurrentAge(4.5F);
    m_star->RecalculateProperties();
    EXPECT_TRUE(m_star->IsMainSequence());

    // Old star has left main sequence
    m_star->SetCurrentAge(12.0F);
    m_star->RecalculateProperties();
    EXPECT_FALSE(m_star->IsMainSequence());
}

//=================================================================================================
// Color Tests
//=================================================================================================

TEST_F(StarTest, SunYellowWhiteColor)
{
    m_star->SetMass(1.0F);
    m_star->RecalculateProperties();

    Utils::ColorRGBF color = m_star->GetColorRGB();

    // Sun should be yellowish-white (high R, high G, moderate B)
    EXPECT_GT(color.r, 0.9F);
    EXPECT_GT(color.g, 0.8F);
    EXPECT_GT(color.b, 0.7F);
}

TEST_F(StarTest, RedDwarfRedColor)
{
    m_star->SetMass(0.2F);
    m_star->RecalculateProperties();

    Utils::ColorRGBF color = m_star->GetColorRGB();

    // Red dwarf should be reddish (high R, low G and B)
    EXPECT_GT(color.r, 0.8F);
    EXPECT_LT(color.g, 0.6F);
    EXPECT_LT(color.b, 0.4F);
}

TEST_F(StarTest, BlueStarBlueColor)
{
    m_star->SetMass(15.0F);
    m_star->RecalculateProperties();

    Utils::ColorRGBF color = m_star->GetColorRGB();

    // Blue star should be bluish (low R, low G, high B)
    EXPECT_LT(color.r, 0.6F);
    EXPECT_LT(color.g, 0.7F);
    EXPECT_GT(color.b, 0.9F);
}

TEST_F(StarTest, ColorComponentsInValidRange)
{
    // Test various temperatures
    for (float32_t temp = 1000.0F; temp <= 40000.0F; temp += 5000.0F)
    {
        // Set mass to produce desired temperature (approximate)
        float32_t mass = std::pow(temp / 5776.0F, 2.5F);
        m_star->SetMass(mass);
        m_star->RecalculateProperties();

        Utils::ColorRGBF color = m_star->GetColorRGB();

        // All color components should be in [0, 1]
        EXPECT_GE(color.r, 0.0F);
        EXPECT_LE(color.r, 1.0F);
        EXPECT_GE(color.g, 0.0F);
        EXPECT_LE(color.g, 1.0F);
        EXPECT_GE(color.b, 0.0F);
        EXPECT_LE(color.b, 1.0F);
    }
}

//=================================================================================================
// Edge Case Tests
//=================================================================================================

TEST_F(StarTest, VeryLowMassStar)
{
    // Minimum viable star mass (~0.08 Msol)
    m_star->SetMass(0.08F);
    m_star->RecalculateProperties();

    EXPECT_GT(m_star->GetLuminosity(), 0.0F);
    EXPECT_GT(m_star->GetRadius(), 0.0F);
    EXPECT_GT(m_star->GetTemperature(), 1000.0F);
}

TEST_F(StarTest, VeryHighMassStar)
{
    // Near upper mass limit (~100 Msol)
    m_star->SetMass(100.0F);
    m_star->RecalculateProperties();

    EXPECT_GT(m_star->GetLuminosity(), 100000.0F);
    EXPECT_GT(m_star->GetRadius(), 10.0F);
    EXPECT_GT(m_star->GetTemperature(), 20000.0F);
}

//=================================================================================================
// Property Recalculation Tests
//=================================================================================================

TEST_F(StarTest, RecalculateUpdatesAllProperties)
{
    // Change mass
    m_star->SetMass(2.0F);

    // Get properties before recalculation
    float32_t old_luminosity = m_star->GetLuminosity();

    // Recalculate
    m_star->RecalculateProperties();

    // Properties should change
    EXPECT_NE(m_star->GetLuminosity(), old_luminosity);
}

TEST_F(StarTest, PropertiesConsistentAcrossRecalculations)
{
    m_star->SetMass(1.5F);

    // Recalculate twice
    m_star->RecalculateProperties();
    float32_t luminosity1 = m_star->GetLuminosity();
    float32_t temperature1 = m_star->GetTemperature();

    m_star->RecalculateProperties();
    float32_t luminosity2 = m_star->GetLuminosity();
    float32_t temperature2 = m_star->GetTemperature();

    // Should be identical
    EXPECT_FLOAT_EQ(luminosity1, luminosity2);
    EXPECT_FLOAT_EQ(temperature1, temperature2);
}

} // namespace Orogena::Stellar