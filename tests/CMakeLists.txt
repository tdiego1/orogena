# Find Google Test
find_package(GTest REQUIRED)
include(GoogleTest)

# Unit tests
add_executable(test_unit
    unit/test_voronoi.cpp
    unit/test_plate_physics.cpp
)

target_link_libraries(test_unit
    PRIVATE
        GTest::gtest_main
        orogena_global
        orogena_regional
)

gtest_discover_tests(test_unit)

# Integration tests
add_executable(test_integration
    integration/test_multiscale_pipeline.cpp
)

target_link_libraries(test_integration
    PRIVATE
        GTest::gtest_main
        orogena_global
        orogena_regional
        orogena_local
)

gtest_discover_tests(test_integration)
