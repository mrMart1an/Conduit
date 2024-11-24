cmake_minimum_required(VERSION 3.2)

set(ENGINE_NAME "Conduit")

project(
    ${ENGINE_NAME}
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------
# Build options
# --------

option(BUILD_EXAMPLES "Build the library examples" ON)
option(BUILD_TESTS "Build the library tests" ON)

option(VULKAN_BACKEND "Enable the Vulkan rendering backend" ON)
option(OPENGL_BACKEND "Enable the OpenGL rendering backend" ON)

option(GLFW_ENABLE "Use Glfw as the windowing backend" ON)

set(LOG_LEVEL "Trace" CACHE STRING "Conduit engine log level")
set_property(
    CACHE LOG_LEVEL PROPERTY STRINGS
    Trace Debug Info Warning Error Fatal
) 

# Operative system
if(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX ON)
elseif(WIN32)
    set(PLATFORM_WINDOWS ON)
endif()

# If the host machine is not using a supported OS halt the build process
if (NOT PLATFORM_LINUX AND NOT PLATFORM_WINDOWS)
    message(FATAL_ERROR
        "Unsupported platform, the engine only support Linux and Windows"
    )
endif()

# If both the graphics backends are disable halt the build process
if (NOT VULKAN_BACKEND AND NOT OPENGL_BACKEND)
    message(FATAL_ERROR "At least one graphics backend need to be enabled")
endif()

if (NOT GLFW_ENABLE)
    message(FATAL_ERROR "At least one windowing backend need to be enabled")
endif()

message("Vulkan rendering backend: ${VULKAN_BACKEND}")
message("OpenGL rendering backend: ${OPENGL_BACKEND}")

message("Glfw windowing backend: ${GLFW_ENABLE}")

message("Engine compile time log level: ${LOG_LEVEL}")

# --------
# Engine configuration
# --------

# Operative system
if(PLATFORM_LINUX)
    set(CNDT_PLATFORM_LINUX ON)
elseif(PLATFORM_WINDOWS)
    set(CNDT_PLATFORM_WINDOWS ON)
endif()

# Rendering backend
if(VULKAN_BACKEND)
    set(CNDT_VULKAN_BACKEND ON)
endif()
if(OPENGL_BACKEND)
    set(CNDT_OPENGL_BACKEND ON)
endif()

# Windowing backend
if(GLFW_ENABLE)
    set(CNDT_GLFW_WINDOWING_BACKEND ON)
endif()

# Engine name
set(CNDT_ENGINE_NAME ${PROJECT_NAME})

# Engine version
set(CNDT_ENGINE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set(CNDT_ENGINE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set(CNDT_ENGINE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})

# Logger level
if (${LOG_LEVEL} STREQUAL "Trace")
    set(CNDT_CORE_LOGGER_LEVEL 0)
elseif (${LOG_LEVEL} STREQUAL "Debug")
    set(CNDT_CORE_LOGGER_LEVEL 1)
elseif (${LOG_LEVEL} STREQUAL "Info")
    set(CNDT_CORE_LOGGER_LEVEL 2)
elseif (${LOG_LEVEL} STREQUAL "Warning")
    set(CNDT_CORE_LOGGER_LEVEL 3)
elseif (${LOG_LEVEL} STREQUAL "Error")
    set(CNDT_CORE_LOGGER_LEVEL 4)
elseif (${LOG_LEVEL} STREQUAL "Fatal")
    set(CNDT_CORE_LOGGER_LEVEL 5)
else()
    message(SEND_ERROR "Unrecognised log level, defaulting to Trace")
    set(CNDT_CORE_LOGGER_LEVEL 0)
endif()

# Create the header configuration file
configure_file(buildConfig.h.in buildConfig.h)

# Include build configuration file

# --------
# Engine static library target
# --------

add_subdirectory("./extern/glm")
add_subdirectory("./extern/glfw")
add_subdirectory("./extern/SPIRV-Cross")
add_subdirectory("./extern/VulkanMemoryAllocator")
add_subdirectory("./extern/fmt")
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("./extern/json")
add_subdirectory("./extern/tinyobjloader")
add_subdirectory("./extern/glad")

# Find graphics dependency
# Vulkan is always necessary for shaders compilation
find_package(Vulkan REQUIRED)
if (VULKAN_BACKEND)
    set(VULKAN_LIB Vulkan::Vulkan)
endif()

if (OPENGL_BACKEND)
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIB OpenGL::GL)
endif()

add_subdirectory("src")
add_library(${PROJECT_NAME} STATIC ${ENGINE_SRC})

target_precompile_headers(${PROJECT_NAME} PUBLIC "pch/stdlibPch.h")
target_precompile_headers(${PROJECT_NAME} PUBLIC "pch/libPch.h")
target_precompile_headers(${PROJECT_NAME} PUBLIC "pch/cndtPch.h")
    
# TODO: Remove vulkan and OpenGL dependency if the backend are disabled
target_link_libraries(${PROJECT_NAME}
    PRIVATE glfw
    PRIVATE ${VULKAN_LIB}
    PRIVATE ${OPENGL_LIB}
    PRIVATE spirv-cross-core
    PRIVATE GPUOpen::VulkanMemoryAllocator
    PRIVATE glad
    PRIVATE tinyobjloader
    
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC fmt::fmt
    PUBLIC glm
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE "src"

    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}"
    PUBLIC "include"
)

# --------
# Prepare the builtin resources 
# --------

add_subdirectory("resources")
add_dependencies(${PROJECT_NAME} builtin_res)

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

