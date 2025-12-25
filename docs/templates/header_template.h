/**************************************************************************************************/
/**
 * @file component_class_name.h
 * @brief Brief one-line description of what this class does
 *
 * @details
 * Longer description explaining the purpose, responsibilities, and design decisions.
 * Include usage examples if helpful.
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

// System includes
#include <memory>
#include <vector>

// Qt includes
#include <QString>

// Third-party includes
#include <glm/glm.hpp>

// Project includes
#include "other_component/other_class.h"

namespace Orogena
{
namespace ComponentName
{

/**************************************************************************************************/
/**
 * @brief Brief description of the class
 *
 * @details
 * Detailed explanation of what this class does, its responsibilities,
 * and key design decisions. Mention important invariants or constraints.
 *
 * Example usage:
 * @code
 * ClassName instance;
 * instance.DoSomething();
 * @endcode
 */
class ClassName
{
public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    enum class Status
    {
        IDLE,
        RUNNING,
        COMPLETED
    };

    struct Config
    {
        int maxIterations;
        double tolerance;
    };

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    /**
     * @brief Default constructor
     */
    ClassName();

    /**
     * @brief Construct with configuration
     * @param config Configuration parameters
     */
    explicit ClassName(const Config& config);

    /**
     * @brief Destructor
     */
    ~ClassName();

    // Delete copy operations (or implement if needed)
    ClassName(const ClassName&) = delete;
    ClassName& operator=(const ClassName&) = delete;

    // Default move operations (or implement if needed)
    ClassName(ClassName&&) = default;
    ClassName& operator=(ClassName&&) = default;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Brief description of method
     *
     * @param paramName Description of parameter
     * @return Description of return value
     *
     * @throws std::invalid_argument if paramName is out of range
     *
     * @note Important usage notes or constraints
     * @see RelatedMethod()
     */
    int DoSomething(int paramName);

    /**
     * @brief Get current status
     * @return Current status
     */
    Status GetStatus() const;

    /**
     * @brief Check if operation is complete
     * @return true if complete, false otherwise
     */
    bool IsComplete() const;

    //=============================================================================================
    // Public Members
    //=============================================================================================

    // (Usually avoid public members - prefer getters/setters)

protected:
    //=============================================================================================
    // Protected Types
    //=============================================================================================

    // (Add protected types if needed for derived classes)

    //=============================================================================================
    // Protected Functions
    //=============================================================================================

    /**
     * @brief Protected helper method
     */
    void ProtectedHelper();

    //=============================================================================================
    // Protected Members
    //=============================================================================================

    int m_ProtectedValue;

private:
    //=============================================================================================
    // Private Types
    //=============================================================================================

    struct InternalData
    {
        double value;
        int counter;
    };

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    /**
     * @brief Internal calculation helper
     */
    void CalculateInternals();

    /**
     * @brief Validate configuration
     */
    bool ValidateConfig() const;

    //=============================================================================================
    // Private Members
    //=============================================================================================

    // Configuration
    Config m_Config;

    // State
    Status m_Status;
    bool m_IsInitialized;

    // Data
    std::vector<double> m_Data;
    std::unique_ptr<InternalData> m_InternalData;

    // Static members
    static int s_InstanceCount;
};

} // namespace ComponentName
} // namespace Orogena
