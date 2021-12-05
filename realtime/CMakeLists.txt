project(luna.realtime)

add_library(${PROJECT_NAME}
    "AsioUdpInputOutput.cpp"
    "AsioTimer.cpp"
    "DiscoveryResponder.cpp"
    "DirectController.cpp"
    "RealtimeStreamer.cpp"
    "RealtimePlugin.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC lunaProto luna.core luna.generators idf::freertos idf::asio
)
