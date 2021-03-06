add_subdirectory(tests)
add_subdirectory(compensation_signal)

set(outfile ${CMAKE_CURRENT_BINARY_DIR}/mesh_impulse_response.h)
add_custom_command(OUTPUT ${outfile}
    COMMAND write_compensation_signal 512 > ${outfile}
)

file(GLOB_RECURSE sources "src/*.cpp")
file(GLOB_RECURSE public_headers "include/*.h")
file(GLOB_RECURSE private_headers "src/*.h")

add_library(waveguide STATIC ${sources} ${public_headers} ${private_headers} ${outfile})

add_dependencies(waveguide glm_external)

target_include_directories(waveguide
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE
    ${DEPENDENCY_INSTALL_PREFIX}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(waveguide core samplerate ${ITPP_LIBRARIES})
