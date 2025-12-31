/**************************************************************************************************/
/**
 * @file gui_qt_settings.cpp
 * @brief Qt-based implementation of ISettings interface
 *
 * @author Diego Torres
 * @date 2025
 * @copyright Copyright (C) 2025 Diego Torres. All rights reserved.
 */
/**************************************************************************************************/

#include "gui_qt_settings.h"
#include <QStringList>

namespace Orogena::GUI
{

//=============================================================================================
// Constructors
//=============================================================================================

QtSettings::QtSettings()
    : m_Settings{"Orogena", "Orogena"}
{
}

QtSettings::QtSettings(const std::string& organization, const std::string& application)
    : m_Settings{QString::fromStdString(organization), QString::fromStdString(application)}
{
}

//=============================================================================================
// String Settings
//=============================================================================================

void QtSettings::SetString(const std::string& key, const std::string& value)
{
    m_Settings.setValue(QString::fromStdString(key), QString::fromStdString(value));
}

std::optional<std::string> QtSettings::GetString(const std::string& key) const
{
    QVariant value = m_Settings.value(QString::fromStdString(key));
    if (!value.isValid())
    {
        return std::nullopt;
    }
    return value.toString().toStdString();
}

//=============================================================================================
// Integer Settings
//=============================================================================================

void QtSettings::SetInt(const std::string& key, int32_t value)
{
    m_Settings.setValue(QString::fromStdString(key), value);
}

std::optional<int32_t> QtSettings::GetInt(const std::string& key) const
{
    QVariant value = m_Settings.value(QString::fromStdString(key));
    if (!value.isValid())
    {
        return std::nullopt;
    }

    bool ok = false;
    int32_t result = value.toInt(&ok);
    if (!ok)
    {
        return std::nullopt;
    }
    return result;
}

//=============================================================================================
// 64-bit Integer Settings
//=============================================================================================

void QtSettings::SetInt64(const std::string& key, int64_t value)
{
    m_Settings.setValue(QString::fromStdString(key), static_cast<qlonglong>(value));
}

std::optional<int64_t> QtSettings::GetInt64(const std::string& key) const
{
    QVariant value = m_Settings.value(QString::fromStdString(key));
    if (!value.isValid())
    {
        return std::nullopt;
    }

    bool ok = false;
    int64_t result = value.toLongLong(&ok);
    if (!ok)
    {
        return std::nullopt;
    }
    return result;
}

//=============================================================================================
// Floating-point Settings
//=============================================================================================

void QtSettings::SetFloat(const std::string& key, float64_t value)
{
    m_Settings.setValue(QString::fromStdString(key), value);
}

std::optional<float64_t> QtSettings::GetFloat(const std::string& key) const
{
    QVariant value = m_Settings.value(QString::fromStdString(key));
    if (!value.isValid())
    {
        return std::nullopt;
    }

    bool ok = false;
    float64_t result = value.toDouble(&ok);
    if (!ok)
    {
        return std::nullopt;
    }
    return result;
}

//=============================================================================================
// Boolean Settings
//=============================================================================================

void QtSettings::SetBool(const std::string& key, bool value)
{
    m_Settings.setValue(QString::fromStdString(key), value);
}

std::optional<bool> QtSettings::GetBool(const std::string& key) const
{
    QVariant value = m_Settings.value(QString::fromStdString(key));
    if (!value.isValid())
    {
        return std::nullopt;
    }
    return value.toBool();
}

//=============================================================================================
// String List Settings
//=============================================================================================

void QtSettings::SetStringList(const std::string& key, const std::vector<std::string>& value)
{
    QStringList qList;
    qList.reserve(static_cast<int>(value.size()));
    for (const auto& str : value)
    {
        qList.append(QString::fromStdString(str));
    }
    m_Settings.setValue(QString::fromStdString(key), qList);
}

std::optional<std::vector<std::string>> QtSettings::GetStringList(const std::string& key) const
{
    QVariant value = m_Settings.value(QString::fromStdString(key));
    if (!value.isValid())
    {
        return std::nullopt;
    }

    QStringList qList = value.toStringList();
    std::vector<std::string> result;
    result.reserve(static_cast<size_t>(qList.size()));
    for (const auto& str : qList)
    {
        result.push_back(str.toStdString());
    }
    return result;
}

//=============================================================================================
// Utility Methods
//=============================================================================================

bool QtSettings::Contains(const std::string& key) const
{
    return m_Settings.contains(QString::fromStdString(key));
}

void QtSettings::Remove(const std::string& key)
{
    m_Settings.remove(QString::fromStdString(key));
}

void QtSettings::Clear()
{
    m_Settings.clear();
}

void QtSettings::Sync()
{
    m_Settings.sync();
}

//=============================================================================================
// Group Management
//=============================================================================================

void QtSettings::BeginGroup(const std::string& prefix)
{
    m_Settings.beginGroup(QString::fromStdString(prefix));
}

void QtSettings::EndGroup()
{
    m_Settings.endGroup();
}

} // namespace Orogena::GUI
