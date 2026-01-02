/**************************************************************************************************/
/**
 * @file core_settings_interface.h
 * @brief Abstract interface for application settings
 *
 * @details
 * Defines a pure C++ interface for settings management, keeping Qt dependencies out of core logic.
 * Implementations can use QSettings or other backends while maintaining framework independence.
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

#pragma once

#include <string>
#include <vector>

#include "utils/utils_types.h"

#include <optional>

namespace Orogena::Core
{

/**
 * @brief Abstract interface for application settings
 *
 * @details
 * Provides a framework-independent interface for reading and writing application settings.
 * All methods use standard C++ types (std::string, std::optional) to avoid Qt dependencies.
 *
 * Settings are organized by keys (e.g., "window/geometry", "simulation/plateCount").
 * Missing values return std::nullopt to distinguish between "not set" and "set to default".
 *
 * Thread Safety: Implementation-dependent. Qt-based implementations are thread-safe.
 */
class ISettings
{
  public:
    virtual ~ISettings() = default;

    // String settings
    virtual void SetString(const std::string& key, const std::string& value) = 0;
    virtual std::optional<std::string> GetString(const std::string& key) const = 0;

    // Integer settings
    virtual void                   SetInt(const std::string& key, int32_t value) = 0;
    virtual std::optional<int32_t> GetInt(const std::string& key) const = 0;

    // 64-bit integer settings
    virtual void                   SetInt64(const std::string& key, int64_t value) = 0;
    virtual std::optional<int64_t> GetInt64(const std::string& key) const = 0;

    // Floating-point settings
    virtual void                     SetFloat(const std::string& key, float64_t value) = 0;
    virtual std::optional<float64_t> GetFloat(const std::string& key) const = 0;

    // Boolean settings
    virtual void                SetBool(const std::string& key, bool value) = 0;
    virtual std::optional<bool> GetBool(const std::string& key) const = 0;

    // String list settings (e.g., recent projects)
    virtual void SetStringList(const std::string& key, const std::vector<std::string>& value) = 0;
    virtual std::optional<std::vector<std::string>> GetStringList(const std::string& key) const = 0;

    // Utility methods
    virtual bool Contains(const std::string& key) const = 0;
    virtual void Remove(const std::string& key) = 0;
    virtual void Clear() = 0;
    virtual void Sync() = 0; ///< Force write to persistent storage

    // Group management (for hierarchical settings)
    virtual void BeginGroup(const std::string& prefix) = 0;
    virtual void EndGroup() = 0;

    // Standard paths (cross-platform)

    /**
     * @brief Get the default directory for user projects
     * @return Path to default projects directory (e.g., ~/Documents/Orogena)
     * @details Creates the directory if it doesn't exist
     */
    virtual std::string GetDefaultProjectsDirectory() const = 0;
};

} // namespace Orogena::Core
