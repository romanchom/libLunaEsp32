project(luna.core)

add_library(${PROJECT_NAME}
    "Configurable.cpp"
    "EventLoop.cpp"
    "IdleController.cpp"
    "ControllerMux.cpp"
    "TlsConfiguration.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC idf::freertos prism Eigen3::Eigen lunaProto mbedtls-cpp
)
