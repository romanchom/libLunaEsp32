project(luna.update)

add_library(${PROJECT_NAME}
    "UpdatePlugin.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core
    PRIVATE idf::freertos idf::app_update
)

if(IDF_TARGET STREQUAL "esp32")
    target_sources(${PROJECT_NAME}
        PRIVATE "UpdaterAsio.cpp" "AsioTcpInputOutput.cpp"
    )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE idf::asio
    )

else()
    target_sources(${PROJECT_NAME}
        PRIVATE "UpdaterSocket.cpp"
    )
endif()