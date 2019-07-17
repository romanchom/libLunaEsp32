project(luna.mqtt)

add_library(${PROJECT_NAME}
    "Client.cpp"
    "Effect.cpp"
    "Service.cpp"
    "EffectMixer.cpp"
    "ConstantEffect.cpp"
    "FlameEffect.cpp"
    "PlasmaEffect.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna/mqtt"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core luna.generators idf::freertos idf::mqtt idf::asio
)
