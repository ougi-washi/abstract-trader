Message("Generating shaders")
set(SHADER_SOURCE_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
file(GLOB SHADER_FILES "${SHADER_SOURCE_DIR}/*.glsl")
set(SHADER_HEADERS)

foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    set(SHADER_HEADER ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.h)
    list(APPEND SHADER_HEADERS ${SHADER_HEADER})
    add_custom_command(
        OUTPUT ${SHADER_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo "const char* ${SHADER_NAME}_shader = R\"shader(" > ${SHADER_HEADER}
        COMMAND ${CMAKE_COMMAND} -E cat ${SHADER_FILE} >> ${SHADER_HEADER}
        COMMAND ${CMAKE_COMMAND} -E echo ")shader\";" >> ${SHADER_HEADER}
        DEPENDS ${SHADER_FILE}
        COMMENT "Generating header for ${SHADER_NAME}.glsl"
    )
endforeach()

add_custom_target(shaders DEPENDS ${SHADER_HEADERS})
