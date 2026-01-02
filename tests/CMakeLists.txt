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
