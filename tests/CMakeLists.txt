include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

macro(cndt_add_test TEST_NAME)
    cndt_executable(
        ${TEST_NAME} ${ARGN} 
        LIBRARIES 
            GTest::gtest 
            GTest::gtest_main 
            GTest::gmock 
            GTest::gmock_main
    )

    gtest_discover_tests(${TEST_NAME}
         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
endmacro()

# Add the tests
add_subdirectory("events")
add_subdirectory("ecs")
