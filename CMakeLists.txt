project(libLunaEsp32)

find_package(Eigen3 REQUIRED)

if(IDF_TARGET STREQUAL "esp32")
    add_compile_definitions(ESP32=1)
elseif(IDF_TARGET STREQUAL "esp8266")
    add_compile_definitions(ESP8266=1)
endif()

add_library(${PROJECT_NAME}
    "WiFi.cpp"
    "Nvs.cpp"
    "Luna.cpp"
    "PersistencyPlugin.cpp"
)

add_subdirectory(prism)
add_subdirectory(lunaProto)
add_subdirectory(mbedtls-cpp)
add_subdirectory(core)
add_subdirectory(generators)
add_subdirectory(mqtt)

target_link_libraries(${PROJECT_NAME} PUBLIC
    idf::nvs_flash
    luna.core
    luna.generators
    # luna.mqtt
)

if(IDF_TARGET STREQUAL "esp32")
    target_sources(${PROJECT_NAME}
        PRIVATE "OnlineContext.cpp"
    )

    add_subdirectory(driver)
    add_subdirectory(realtime)
    add_subdirectory(update)
    add_subdirectory(infrared)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        luna.driver
        luna.realtime
        luna.update
        luna.infrared
    )
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/luna"
)
