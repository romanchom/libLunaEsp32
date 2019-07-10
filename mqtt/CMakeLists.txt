project(luna.mqtt)

add_library(${PROJECT_NAME}
    "ConstantMqttEffect.cpp"
    "FlameMqttEffect.cpp"
    "MqttClient.cpp"
    "MqttService.cpp"
    "PlasmaMqttEffect.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core luna.generators idf::freertos idf::mqtt idf::asio
)
