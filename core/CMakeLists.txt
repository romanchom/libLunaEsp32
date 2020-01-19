project(luna.core)

add_library(${PROJECT_NAME}
    "Service.cpp"
    "ServiceManager.cpp"
    "Configurable.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC prism Eigen3::Eigen lunaProto idf::nvs_flash
)
