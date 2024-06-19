include(GoogleTest)

add_subdirectory(
    "${PROJECT_SOURCE_DIR}/extern/googletest"
    "extern/googletest"
)

macro(cndt_add_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    
    target_link_libraries(
        ${TEST_NAME}
        ${PROJECT_NAME}
        gtest gmock gtest_main
    )
    
    gtest_discover_tests(${TEST_NAME}
         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
endmacro()

# Add the tests
add_subdirectory("events")
add_subdirectory("ecs")
