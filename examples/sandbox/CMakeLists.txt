set(APP_NAME "Sandbox")

cndt_executable(${APP_NAME} "src/sandbox.cpp")

#add_custom_target(copy_shader ALL
#    COMMAND ${CMAKE_COMMAND} -E copy_directory 
#    ${PROJECT_BINARY_DIR}/resources 
#    ${CMAKE_CURRENT_BINARY_DIR}/resources
#    DEPENDS ${ENGINE_NAME}
#)

#add_dependencies(${APP_NAME} copy_shader)
