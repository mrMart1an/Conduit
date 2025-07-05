add_custom_target(builtin_res ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_SOURCE_DIR}/resources 
    ${CMAKE_BINARY_DIR}/resources
)

# Compile the builtin shader
add_subdirectory("shaders/vulkan")
add_dependencies(builtin_res build_shaders)
