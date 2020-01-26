idf_component_register(
    SRCS
        "AsioTcpInputOutput.cpp"
        "WiFi.cpp"
        "Updater.cpp"
        "Nvs.cpp"
        "Luna.cpp"
        "OnlineContext.cpp"
        "Infrared.cpp"
        "EffectsPlugin.cpp"
        "UpdatePlugin.cpp"
        "RealtimePlugin.cpp"
        "PersistencyPlugin.cpp"
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
add_subdirectory(realtime)

target_link_libraries(${COMPONENT_TARGET} PUBLIC
    lunaProto
    prism
    mbedtls-cpp
    luna.core
    luna.driver
    luna.generators
    luna.mqtt
    luna.realtime
)

target_include_directories(${COMPONENT_TARGET}
    PRIVATE include/luna
)
