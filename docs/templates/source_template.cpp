/**************************************************************************************************/
/**
 * @file component_class_name.cpp
 * @brief Implementation of ClassName
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

#include "component_class_name.h"

// Additional implementation-only includes
#include <algorithm>

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace Orogena
{
namespace ComponentName
{

//=================================================================================================
// Static Member Initialization
//=================================================================================================

int ClassName::s_InstanceCount = 0;

//=================================================================================================
// Constructors/Destructor
//=================================================================================================

/**************************************************************************************************/
ClassName::ClassName()
    : m_Config{},
      m_Status(Status::IDLE),
      m_IsInitialized(false),
      m_InternalData(std::make_unique<InternalData>())
{
    ++s_InstanceCount;
    spdlog::debug("ClassName constructed (instance #{})", s_InstanceCount);
}

/**************************************************************************************************/
ClassName::ClassName(const Config& config)
    : m_Config(config),
      m_Status(Status::IDLE),
      m_IsInitialized(false),
      m_InternalData(std::make_unique<InternalData>())
{
    if (!ValidateConfig())
    {
        throw std::invalid_argument("Invalid configuration");
    }

    ++s_InstanceCount;
    spdlog::debug("ClassName constructed with config (instance #{})", s_InstanceCount);
}

/**************************************************************************************************/
ClassName::~ClassName()
{
    --s_InstanceCount;
    spdlog::debug("ClassName destroyed (remaining instances: {})", s_InstanceCount);
}

//=================================================================================================
// Public Functions
//=================================================================================================

/**************************************************************************************************/
int ClassName::DoSomething(int paramName)
{
    if (paramName < 0)
    {
        throw std::invalid_argument("paramName must be non-negative");
    }

    spdlog::debug("DoSomething called with paramName={}", paramName);

    CalculateInternals();

    m_Status = Status::RUNNING;

    int result = paramName * 2;

    m_Status = Status::COMPLETED;

    return result;
}

/**************************************************************************************************/
ClassName::Status ClassName::GetStatus() const
{
    return m_Status;
}

/**************************************************************************************************/
bool ClassName::IsComplete() const
{
    return m_Status == Status::COMPLETED;
}

//=================================================================================================
// Protected Functions
//=================================================================================================

/**************************************************************************************************/
void ClassName::ProtectedHelper()
{
    // Protected method implementation
    spdlog::trace("ProtectedHelper called");
}

//=================================================================================================
// Private Functions
//=================================================================================================

/**************************************************************************************************/
void ClassName::CalculateInternals()
{
    // Internal calculation logic
    m_InternalData->value = 3.14159;
    m_InternalData->counter++;

    spdlog::trace("Internal calculation: value={}, counter={}", m_InternalData->value,
                  m_InternalData->counter);
}

/**************************************************************************************************/
bool ClassName::ValidateConfig() const
{
    if (m_Config.maxIterations <= 0)
    {
        spdlog::error("Invalid config: maxIterations must be > 0");
        return false;
    }

    if (m_Config.tolerance < 0.0)
    {
        spdlog::error("Invalid config: tolerance must be >= 0");
        return false;
    }

    return true;
}

} // namespace ComponentName
} // namespace Orogena
