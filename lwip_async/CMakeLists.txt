project(lwip_async)

add_library(${PROJECT_NAME}
    "AsioTcpInputOutput.cpp"
    "DtlsInputOutput.cpp"
    "PbufStream.cpp"
    "Timer.cpp"
)

target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
