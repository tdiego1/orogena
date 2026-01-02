/**************************************************************************************************/
/**
 * @file core_application.h
 * @brief Main application controller.
 *
 * @details
 * Manages application lifecycle, initialization, and global state.
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

namespace Orogena
{
namespace Core
{

/**************************************************************************************************/
/**
 * @brief Main application controller
 *
 * @details
 * Manages application lifecycle, initialization, and global state.
 */
class Application
{
  public:
    //=============================================================================================
    // Public Types
    //=============================================================================================

    //=============================================================================================
    // Constructors/Destructor
    //=============================================================================================

    Application(void);
    ~Application(void);

    // Delete copy operations
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Default move operations
    Application(Application&&) = default;
    Application& operator=(Application&&) = default;

    //=============================================================================================
    // Public Functions
    //=============================================================================================

    /**
     * @brief Initialize the application.
     */
    void Initialize(void);

    /**
     * @brief Shutdown the application.
     */
    void Shutdown(void);

    //=============================================================================================
    // Public Members
    //=============================================================================================

  private:
    //=============================================================================================
    // Private Types
    //=============================================================================================

    //=============================================================================================
    // Private Functions
    //=============================================================================================

    //=============================================================================================
    // Private Members
    //=============================================================================================
};

} // namespace Core
} // namespace Orogena
