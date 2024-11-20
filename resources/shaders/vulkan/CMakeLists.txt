set(SHADERS_IN_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SHADERS_OUT_DIR "${CMAKE_BINARY_DIR}/resources/shaders/vulkan")

set(SHADERS 
	"${SHADERS_IN_DIR}/builtin.vert" 
	"${SHADERS_IN_DIR}/builtin.frag"
)

file(MAKE_DIRECTORY ${SHADERS_OUT_DIR})

foreach(SHADER ${SHADERS})
	get_filename_component(SHADER_NAME ${SHADER} NAME)
	set(SHADER_OUT_NAME "${SHADERS_OUT_DIR}/${SHADER_NAME}.spv")
	list(APPEND SHADER_OUT_NAMES ${SHADER_OUT_NAME})
	
	add_custom_command(
		MAIN_DEPENDENCY ${SHADER}
		OUTPUT ${SHADER_OUT_NAME}
		COMMAND Vulkan::glslc  ${SHADER} "-o" ${SHADER_OUT_NAME}
		VERBATIM
	)
endforeach()

add_custom_target(build_shaders DEPENDS ${SHADER_OUT_NAMES})
