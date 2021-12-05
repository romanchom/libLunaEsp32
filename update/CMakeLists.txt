project(luna.update)

add_library(${PROJECT_NAME}
    "AsioTcpInputOutput.cpp"
    "Updater.cpp"
    "UpdatePlugin.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core
    PRIVATE idf::freertos idf::asio idf::app_update
)
