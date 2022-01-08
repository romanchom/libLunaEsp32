project(luna.driver)


add_library(${PROJECT_NAME}
    "PWMLight.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core lunaProto
)

if(IDF_TARGET STREQUAL "esp32")
    add_subdirectory(ws281x-driver-esp32)

    target_sources(${PROJECT_NAME} PRIVATE
        "ATX.cpp"
        "HardPWM.cpp"
        "StrandWS281x.cpp"
        "WS281xMeter.cpp"
    )

    target_link_libraries(${PROJECT_NAME}
        PUBLIC idf::driver ws281x-driver-esp32
    )
else()
    target_sources(${PROJECT_NAME} PRIVATE
        "SoftPWM.cpp"
    )
endif()

