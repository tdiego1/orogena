/**************************************************************************************************/
/**
 * @file test_qt_settings.cpp
 * @brief Unit tests for QtSettings implementation
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres
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

#include <QCoreApplication>
#include <QSettings>

#include "gui/gui_qt_settings.h"

#include <filesystem>
#include <gtest/gtest.h>

using namespace Orogena::GUI;
using namespace Orogena::Core;

/**************************************************************************************************/
/**
 * @brief Test fixture for QtSettings tests
 *
 * @details
 * Provides setup and teardown for settings testing. Uses a custom organization/application
 * name to avoid interfering with actual application settings.
 */
class QtSettingsTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        // Use unique organization/app name for testing
        m_Settings = std::make_unique<QtSettings>("OrogenaTest", "SettingsTest");

        // Clear any existing test settings
        m_Settings->Clear();
        m_Settings->Sync();
    }

    void TearDown() override
    {
        // Clean up test settings
        if (m_Settings)
        {
            m_Settings->Clear();
            m_Settings->Sync();
        }
    }

    std::unique_ptr<QtSettings> m_Settings;
};

/**************************************************************************************************/
// String Settings Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, SetAndGetString)
{
    // Arrange
    const std::string key = "test/string";
    const std::string value = "Hello, World!";

    // Act
    m_Settings->SetString(key, value);
    auto retrieved = m_Settings->GetString(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

TEST_F(QtSettingsTest, GetNonexistentStringReturnsNullopt)
{
    // Arrange & Act
    auto retrieved = m_Settings->GetString("nonexistent/key");

    // Assert
    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(QtSettingsTest, SetEmptyString)
{
    // Arrange
    const std::string key = "test/empty";
    const std::string value = "";

    // Act
    m_Settings->SetString(key, value);
    auto retrieved = m_Settings->GetString(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

/**************************************************************************************************/
// Integer Settings Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, SetAndGetInt)
{
    // Arrange
    const std::string key = "test/int";
    const int32_t     value = 42;

    // Act
    m_Settings->SetInt(key, value);
    auto retrieved = m_Settings->GetInt(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

TEST_F(QtSettingsTest, GetNonexistentIntReturnsNullopt)
{
    // Arrange & Act
    auto retrieved = m_Settings->GetInt("nonexistent/int");

    // Assert
    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(QtSettingsTest, SetNegativeInt)
{
    // Arrange
    const std::string key = "test/negative";
    const int32_t     value = -12345;

    // Act
    m_Settings->SetInt(key, value);
    auto retrieved = m_Settings->GetInt(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

/**************************************************************************************************/
// 64-bit Integer Settings Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, SetAndGetInt64)
{
    // Arrange
    const std::string key = "test/int64";
    const int64_t     value = 9223372036854775807LL; // Max int64_t

    // Act
    m_Settings->SetInt64(key, value);
    auto retrieved = m_Settings->GetInt64(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

TEST_F(QtSettingsTest, GetNonexistentInt64ReturnsNullopt)
{
    // Arrange & Act
    auto retrieved = m_Settings->GetInt64("nonexistent/int64");

    // Assert
    EXPECT_FALSE(retrieved.has_value());
}

/**************************************************************************************************/
// Floating-point Settings Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, SetAndGetFloat)
{
    // Arrange
    const std::string key = "test/float";
    const float64_t   value = 3.14159265358979;

    // Act
    m_Settings->SetFloat(key, value);
    auto retrieved = m_Settings->GetFloat(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_DOUBLE_EQ(value, retrieved.value());
}

TEST_F(QtSettingsTest, GetNonexistentFloatReturnsNullopt)
{
    // Arrange & Act
    auto retrieved = m_Settings->GetFloat("nonexistent/float");

    // Assert
    EXPECT_FALSE(retrieved.has_value());
}

TEST_F(QtSettingsTest, SetNegativeFloat)
{
    // Arrange
    const std::string key = "test/negative_float";
    const float64_t   value = -2.71828;

    // Act
    m_Settings->SetFloat(key, value);
    auto retrieved = m_Settings->GetFloat(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_DOUBLE_EQ(value, retrieved.value());
}

/**************************************************************************************************/
// Boolean Settings Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, SetAndGetBoolTrue)
{
    // Arrange
    const std::string key = "test/bool_true";

    // Act
    m_Settings->SetBool(key, true);
    auto retrieved = m_Settings->GetBool(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_TRUE(retrieved.value());
}

TEST_F(QtSettingsTest, SetAndGetBoolFalse)
{
    // Arrange
    const std::string key = "test/bool_false";

    // Act
    m_Settings->SetBool(key, false);
    auto retrieved = m_Settings->GetBool(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_FALSE(retrieved.value());
}

TEST_F(QtSettingsTest, GetNonexistentBoolReturnsNullopt)
{
    // Arrange & Act
    auto retrieved = m_Settings->GetBool("nonexistent/bool");

    // Assert
    EXPECT_FALSE(retrieved.has_value());
}

/**************************************************************************************************/
// String List Settings Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, SetAndGetStringList)
{
    // Arrange
    const std::string              key = "test/stringlist";
    const std::vector<std::string> value = {"/path/to/project1.oro", "/path/to/project2.oro",
                                            "/path/to/project3.oro"};

    // Act
    m_Settings->SetStringList(key, value);
    auto retrieved = m_Settings->GetStringList(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

TEST_F(QtSettingsTest, SetEmptyStringList)
{
    // Arrange
    const std::string              key = "test/empty_list";
    const std::vector<std::string> value = {};

    // Act
    m_Settings->SetStringList(key, value);
    auto retrieved = m_Settings->GetStringList(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_TRUE(retrieved.value().empty());
}

TEST_F(QtSettingsTest, GetNonexistentStringListReturnsNullopt)
{
    // Arrange & Act
    auto retrieved = m_Settings->GetStringList("nonexistent/list");

    // Assert
    EXPECT_FALSE(retrieved.has_value());
}

/**************************************************************************************************/
// Utility Methods Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, ContainsReturnsTrueForExistingKey)
{
    // Arrange
    const std::string key = "test/exists";
    m_Settings->SetString(key, "value");

    // Act & Assert
    EXPECT_TRUE(m_Settings->Contains(key));
}

TEST_F(QtSettingsTest, ContainsReturnsFalseForNonexistentKey)
{
    // Arrange & Act & Assert
    EXPECT_FALSE(m_Settings->Contains("nonexistent/key"));
}

TEST_F(QtSettingsTest, RemoveDeletesKey)
{
    // Arrange
    const std::string key = "test/to_remove";
    m_Settings->SetString(key, "value");
    ASSERT_TRUE(m_Settings->Contains(key));

    // Act
    m_Settings->Remove(key);

    // Assert
    EXPECT_FALSE(m_Settings->Contains(key));
}

TEST_F(QtSettingsTest, ClearRemovesAllSettings)
{
    // Arrange
    m_Settings->SetString("key1", "value1");
    m_Settings->SetInt("key2", 42);
    m_Settings->SetBool("key3", true);

    // Act
    m_Settings->Clear();

    // Assert
    EXPECT_FALSE(m_Settings->Contains("key1"));
    EXPECT_FALSE(m_Settings->Contains("key2"));
    EXPECT_FALSE(m_Settings->Contains("key3"));
}

TEST_F(QtSettingsTest, SyncPersistsSettings)
{
    // Arrange
    const std::string key = "test/persist";
    const std::string value = "persistent_value";
    m_Settings->SetString(key, value);

    // Act
    m_Settings->Sync();

    // Create new settings instance to verify persistence
    auto newSettings = std::make_unique<QtSettings>("OrogenaTest", "SettingsTest");
    auto retrieved = newSettings->GetString(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

/**************************************************************************************************/
// Group Management Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, BeginGroupCreatesHierarchy)
{
    // Arrange
    m_Settings->BeginGroup("window");
    m_Settings->SetInt("width", 1024);
    m_Settings->SetInt("height", 768);
    m_Settings->EndGroup();

    // Act
    auto width = m_Settings->GetInt("window/width");
    auto height = m_Settings->GetInt("window/height");

    // Assert
    ASSERT_TRUE(width.has_value());
    ASSERT_TRUE(height.has_value());
    EXPECT_EQ(1024, width.value());
    EXPECT_EQ(768, height.value());
}

TEST_F(QtSettingsTest, NestedGroupsWork)
{
    // Arrange
    m_Settings->BeginGroup("rendering");
    m_Settings->BeginGroup("quality");
    m_Settings->SetString("shadows", "high");
    m_Settings->EndGroup();
    m_Settings->EndGroup();

    // Act
    auto shadows = m_Settings->GetString("rendering/quality/shadows");

    // Assert
    ASSERT_TRUE(shadows.has_value());
    EXPECT_EQ("high", shadows.value());
}

TEST_F(QtSettingsTest, EndGroupRestoresPreviousContext)
{
    // Arrange
    m_Settings->BeginGroup("group1");
    m_Settings->SetInt("value", 1);
    m_Settings->EndGroup();

    m_Settings->BeginGroup("group2");
    m_Settings->SetInt("value", 2);
    m_Settings->EndGroup();

    // Act
    auto value1 = m_Settings->GetInt("group1/value");
    auto value2 = m_Settings->GetInt("group2/value");

    // Assert
    ASSERT_TRUE(value1.has_value());
    ASSERT_TRUE(value2.has_value());
    EXPECT_EQ(1, value1.value());
    EXPECT_EQ(2, value2.value());
}

/**************************************************************************************************/
// Overwrite Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, OverwriteExistingValue)
{
    // Arrange
    const std::string key = "test/overwrite";
    m_Settings->SetInt(key, 42);

    // Act
    m_Settings->SetInt(key, 100);
    auto retrieved = m_Settings->GetInt(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(100, retrieved.value());
}

TEST_F(QtSettingsTest, ChangeValueType)
{
    // Arrange
    const std::string key = "test/type_change";
    m_Settings->SetInt(key, 42);

    // Act - Change from int to string
    m_Settings->SetString(key, "now a string");
    auto retrieved = m_Settings->GetString(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ("now a string", retrieved.value());
}

/**************************************************************************************************/
// Edge Cases
/**************************************************************************************************/

TEST_F(QtSettingsTest, KeyWithSpecialCharacters)
{
    // Arrange
    const std::string key = "test/special/!@#$%/key";
    const std::string value = "special_value";

    // Act
    m_Settings->SetString(key, value);
    auto retrieved = m_Settings->GetString(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

TEST_F(QtSettingsTest, ValueWithUnicodeCharacters)
{
    // Arrange
    const std::string key = "test/unicode";
    const std::string value = "Hello 世界 🌍";

    // Act
    m_Settings->SetString(key, value);
    auto retrieved = m_Settings->GetString(key);

    // Assert
    ASSERT_TRUE(retrieved.has_value());
    EXPECT_EQ(value, retrieved.value());
}

/**************************************************************************************************/
// Standard Paths Tests
/**************************************************************************************************/

TEST_F(QtSettingsTest, GetDefaultProjectsDirectoryReturnsValidPath)
{
    // Act
    std::string path = m_Settings->GetDefaultProjectsDirectory();

    // Assert
    EXPECT_FALSE(path.empty());
    EXPECT_TRUE(std::filesystem::exists(path));
    EXPECT_TRUE(std::filesystem::is_directory(path));
}

TEST_F(QtSettingsTest, GetDefaultProjectsDirectoryContainsOrogena)
{
    // Act
    std::string path = m_Settings->GetDefaultProjectsDirectory();

    // Assert
    EXPECT_NE(path.find("Orogena"), std::string::npos);
}
