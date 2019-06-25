# project(libLunaEsp32)

# set(COMPONENT_SRCS
#     "ATX.cpp"
#     "DiscoveryResponder.cpp"
#     "MqttClient.cpp"
#     "NetworkManager.cpp"
#     "PWM.cpp"
#     "PWMLight.cpp"
#     "StrandWS281x.cpp"
#     "Updater.cpp"
#     "mbedtls-cpp/Exception.cpp"
# )

file(GLOB_RECURSE COMPONENT_SRCS "*.cpp")

set(COMPONENT_ADD_INCLUDEDIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(COMPONENT_REQUIRES "ws281x-driver-esp32 mbedtls app_update mqtt asio")

register_component()

add_subdirectory(lunaProto)
# add_subdirectory(lwip_async)

target_link_libraries(${COMPONENT_TARGET} PUBLIC
    lunaProto
    # lwip_async
)
