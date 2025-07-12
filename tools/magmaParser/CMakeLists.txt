set(TOOL_NAME "magma-parser")

# Find dependency
find_package(Clang REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

# Define source files
set(SOURCE_DIR "src")
set(SOURCE_FILES 
    "${SOURCE_DIR}/main.cpp"
    "${SOURCE_DIR}/argparser.cpp"
    "${SOURCE_DIR}/sourceLoader.cpp"
    "${SOURCE_DIR}/astParser.cpp"
)

add_executable(${TOOL_NAME} ${SOURCE_FILES})

target_link_libraries(${TOOL_NAME} 
    PRIVATE libclang
    PRIVATE argparse::argparse
    PRIVATE nlohmann_json::nlohmann_json
)

target_include_directories(${TOOL_NAME} 
    PRIVATE ${SOURCE_DIR}
)
