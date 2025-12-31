/**************************************************************************************************/
/**
 * @file gui_qt_settings.h
 * @brief Qt-based implementation of ISettings interface
 *
 * @details
 * Provides a QSettings-backed implementation of the ISettings interface.
 * Converts between Qt types (QString, QVariant) and standard C++ types at the boundary.
 *
 * Organization: "Orogena"
 * Application: "Orogena"
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
/**************************************************************************************************/

#pragma once

#include <QSettings>
#include "core/core_settings_interface.h"

namespace Orogena::GUI
{

/**
 * @brief Qt-based settings implementation using QSettings
 *
 * @details
 * Thread-safe implementation backed by QSettings. All Qt types are converted to/from
 * standard C++ types at the boundary to maintain framework independence in core logic.
 *
 * Settings are stored platform-specifically:
 * - Linux: ~/.config/Orogena/Orogena.conf
 * - Windows: HKEY_CURRENT_USER\Software\Orogena\Orogena
 * - macOS: ~/Library/Preferences/com.Orogena.Orogena.plist
 */
class QtSettings : public Core::ISettings
{
public:
    /**
     * @brief Constructs settings with default organization and application name
     */
    QtSettings();

    /**
     * @brief Constructs settings with custom organization and application name
     */
    QtSettings(const std::string& organization, const std::string& application);

    ~QtSettings() override = default;

    // String settings
    void SetString(const std::string& key, const std::string& value) override;
    std::optional<std::string> GetString(const std::string& key) const override;

    // Integer settings
    void SetInt(const std::string& key, int32_t value) override;
    std::optional<int32_t> GetInt(const std::string& key) const override;

    // 64-bit integer settings
    void SetInt64(const std::string& key, int64_t value) override;
    std::optional<int64_t> GetInt64(const std::string& key) const override;

    // Floating-point settings
    void SetFloat(const std::string& key, float64_t value) override;
    std::optional<float64_t> GetFloat(const std::string& key) const override;

    // Boolean settings
    void SetBool(const std::string& key, bool value) override;
    std::optional<bool> GetBool(const std::string& key) const override;

    // String list settings
    void SetStringList(const std::string& key, const std::vector<std::string>& value) override;
    std::optional<std::vector<std::string>> GetStringList(const std::string& key) const override;

    // Utility methods
    bool Contains(const std::string& key) const override;
    void Remove(const std::string& key) override;
    void Clear() override;
    void Sync() override;

    // Group management
    void BeginGroup(const std::string& prefix) override;
    void EndGroup() override;

private:
    QSettings m_Settings;
};

} // namespace Orogena::GUI
