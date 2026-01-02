#[[************************************************************************************************/
/**
 * @brief CMake configuration for the Orogena project tests.
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
/************************************************************************************************]]#

# Find Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# Unit tests
add_executable(test_unit
    unit/core/test_project.cpp
    unit/database/test_database_manager.cpp
    unit/utils/test_logger.cpp
    unit/gui/test_qt_settings.cpp
    unit/stellar/test_star.cpp
)

target_link_libraries(test_unit
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        orogena_stellar
        orogena_core
        orogena_database
        orogena_utils
        orogena_gui
        Qt6::Core
)

gtest_discover_tests(test_unit)

# Integration tests
add_executable(test_integration
)

target_link_libraries(test_integration
    PRIVATE
        GTest::gtest_main
        orogena_global
        orogena_region
        orogena_local
)

gtest_discover_tests(test_integration)
