project(luna.mqtt)

add_library(${PROJECT_NAME}
    "Client.cpp"
    "Codec.cpp"
    "Controller.cpp"
    "MqttPlugin.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core luna.generators idf::freertos idf::mqtt
)
