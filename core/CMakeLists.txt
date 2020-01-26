project(luna.core)

add_library(${PROJECT_NAME}
    "Configurable.cpp"
    "EventLoop.cpp"
    "IdleController.cpp"
    "Controller.cpp"
    "ControllerMux.cpp"
    "TlsConfiguration.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC prism Eigen3::Eigen lunaProto idf::nvs_flash mbedtls-cpp
)
