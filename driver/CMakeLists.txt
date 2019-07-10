project(luna.driver)

add_library(${PROJECT_NAME}
    "ATX.cpp"
    "PWM.cpp"
    "PWMLight.cpp"
    "StrandWS281x.cpp"
    "WS281xMeter.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core idf::driver idf::ws281x-driver-esp32 lunaProto
)
