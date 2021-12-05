project(libLunaEsp32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Eigen3 REQUIRED)

add_subdirectory(prism)
add_subdirectory(mbedtls-cpp)
add_subdirectory(core)
add_subdirectory(driver)
add_subdirectory(generators)
add_subdirectory(mqtt)
add_subdirectory(realtime)
add_subdirectory(update)
add_subdirectory(infrared)

add_library(${PROJECT_NAME}
    "WiFi.cpp"
    "Nvs.cpp"
    "Luna.cpp"
    "OnlineContext.cpp"
    "PersistencyPlugin.cpp"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    idf::nvs_flash
    mbedtls-cpp
    luna.core
    luna.driver
    luna.generators
    luna.mqtt
    luna.realtime
    luna.update
    luna.infrared
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/luna"
)
