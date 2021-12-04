project(libLunaEsp32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")

find_package(Eigen3 REQUIRED)

add_subdirectory(prism)
add_subdirectory(mbedtls-cpp)
add_subdirectory(core)
add_subdirectory(driver)
add_subdirectory(generators)
add_subdirectory(mqtt)
add_subdirectory(realtime)

add_library(${PROJECT_NAME}
    "AsioTcpInputOutput.cpp"
    "WiFi.cpp"
    "Updater.cpp"
    "Nvs.cpp"
    "Luna.cpp"
    "OnlineContext.cpp"
    "Infrared.cpp"
    "UpdatePlugin.cpp"
    "RealtimePlugin.cpp"
    "PersistencyPlugin.cpp"
    "MqttPlugin.cpp"
    "Ir40ButtonRemote.cpp"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    idf::app_update
    idf::asio
    idf::nvs_flash
    mbedtls-cpp
    luna.core
    luna.driver
    luna.generators
    luna.mqtt
    luna.realtime
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/luna"
)
