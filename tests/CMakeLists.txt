# Find Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# Unit tests
add_executable(test_unit
    unit/utils/test_logger.cpp
)

target_link_libraries(test_unit
    PRIVATE
        GTest::gtest_main
        orogena_global
        orogena_region
        orogena_utils
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
