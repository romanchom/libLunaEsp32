# project(libLunaEsp32)
idf_component_register(
    SRCS
        "AsioTcpInputOutput.cpp"
        "AsioUdpInputOutput.cpp"
        "AsioTimer.cpp"

        # "Main.cpp"
        "WiFi.cpp"
        "DiscoveryResponder.cpp"
        "Updater.cpp"

        "RealtimeService.cpp"
        "Nvs.cpp"
        "OnlineContext.cpp"
        "DirectService.cpp"
        "Infrared.cpp"
        "EffectsPlugin.cpp"
        "UpdatePlugin.cpp"
        "Luna.cpp"
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    REQUIRES
        ws281x-driver-esp32 mbedtls app_update mqtt asio nvs_flash
)

find_package(Eigen3 REQUIRED)

add_subdirectory(lunaProto)
add_subdirectory(prism)
add_subdirectory(mbedtls-cpp)
add_subdirectory(core)
add_subdirectory(driver)
add_subdirectory(generators)
add_subdirectory(mqtt)

target_link_libraries(${COMPONENT_TARGET} PUBLIC
    lunaProto
    prism
    mbedtls-cpp
    luna.core
    luna.driver
    luna.generators
    luna.mqtt
)

target_include_directories(${COMPONENT_TARGET}
    PRIVATE include/luna
)
