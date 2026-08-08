add_executable(lodge_tests
        test_laplacian.cpp
        test_divergence.cpp
)

target_link_libraries(lodge_tests
        PRIVATE
        lodge
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(lodge_tests)
