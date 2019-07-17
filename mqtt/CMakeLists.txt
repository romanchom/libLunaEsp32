project(luna.mqtt)

add_library(${PROJECT_NAME}
    "MqttClient.cpp"
    "MqttEffect.cpp"
    "MqttService.cpp"
    "EffectMixer.cpp"
    "ConstantMqttEffect.cpp"
    "FlameMqttEffect.cpp"
    "PlasmaMqttEffect.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core luna.generators idf::freertos idf::mqtt idf::asio
)
