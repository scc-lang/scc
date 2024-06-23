add_executable(scc.cli.test
    commandline_processor_test.cpp
)
target_link_libraries(scc.cli.test
    scc.cli
    GTest::gtest_main
)
gtest_discover_tests(scc.cli.test)