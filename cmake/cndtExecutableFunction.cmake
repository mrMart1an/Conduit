function(cndt_executable EXECUTABLE_NAME)
    # Shift arguments to get source files and libraries
    cmake_parse_arguments(
        PARSE_ARGS 
        "" "" "LIBRARIES" 
        ${ARGN}
    )

    # Ensure source files are provided
    if(NOT PARSE_ARGS_UNPARSED_ARGUMENTS)
        message(
            FATAL_ERROR 
            "cndt_executable: No source files provided for ${EXECUTABLE_NAME}"
        )
    endif()

    # Create the executable
    add_executable(${EXECUTABLE_NAME} ${PARSE_ARGS_UNPARSED_ARGUMENTS})

    # Link the executable to our static library and any additional libraries
    target_link_libraries(
        ${EXECUTABLE_NAME} 
        LINK_PUBLIC ${ENGINE_NAME} 
        ${PARSE_ARGS_LIBRARIES}
    )
endfunction()
