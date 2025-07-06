cmake_minimum_required(VERSION 3.6)

set(ENGINE_NAME "Conduit")

project(
    ${ENGINE_NAME}
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------
# Build options
# --------

option(BUILD_EXAMPLES "Build the library examples" ON)
option(BUILD_TESTS "Build the library tests" ON)

# --------
# Add dependency subdirectories
# --------

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("./extern/glm")
add_subdirectory("./extern/glfw")
add_subdirectory("./extern/SPIRV-Cross")
add_subdirectory("./extern/VulkanMemoryAllocator")
add_subdirectory("./extern/fmt")
add_subdirectory("./extern/json")
add_subdirectory("./extern/tinyobjloader")
add_subdirectory("./extern/glad")

# --------
# Compile engine static library
# --------

add_subdirectory("engine")

# --------
# Include cmake functions
# --------

include("./cmake/cndtExecutableFunction.cmake")

# --------
# Compile the examples
# --------

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_EXAMPLES)
add_subdirectory("examples")
endif()

# --------
# Compile the tests
# --------

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) AND BUILD_TESTS)
    enable_testing()
    add_subdirectory("tests")
endif()

