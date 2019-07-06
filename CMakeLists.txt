# project(libLunaEsp32)

set(COMPONENT_SRCS
    "AsioTcpInputOutput.cpp"
    "AsioUdpInputOutput.cpp"
    "AsioTimer.cpp"

    "ATX.cpp"
    "PWM.cpp"
    "PWMLight.cpp"
    "StrandWS281x.cpp"
    "WS281xMeter.cpp"
    
    "NetworkManager.cpp"
    "DiscoveryResponder.cpp"
    "Updater.cpp"
    
    "Service.cpp"
    "ServiceManager.cpp"
    "IdleService.cpp"
    "MqttService.cpp"
    "RealtimeService.cpp"
    
    "Noise.cpp"
    "ConstantGenerator.cpp"
    "FlameGenerator.cpp"
    "PlasmaGenerator.cpp"

    "FlameMqttEffect.cpp"
    "ConstantMqttEffect.cpp"
    "PlasmaMqttEffect.cpp"
    
    "MqttClient.cpp"
    "mbedtls-cpp/Exception.cpp"
)

set(COMPONENT_ADD_INCLUDEDIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(COMPONENT_REQUIRES "ws281x-driver-esp32 mbedtls app_update mqtt asio")

register_component()

find_package(Eigen3 REQUIRED)

add_subdirectory(lunaProto)
add_subdirectory(prism)

target_link_libraries(${COMPONENT_TARGET} PUBLIC
    lunaProto
    prism
)
