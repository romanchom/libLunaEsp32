# project(libLunaEsp32)

file(GLOB_RECURSE COMPONENT_SRCS  "*.cpp")

set(COMPONENT_ADD_INCLUDEDIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(COMPONENT_REQUIRES "ws281x-driver-esp32 mbedtls")

register_component()

add_subdirectory(lunaProto)

target_link_libraries(${COMPONENT_TARGET} lunaProto)
