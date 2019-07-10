project(mbedtls-cpp)

add_library(${PROJECT_NAME}
    "Exception.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/mbedtls-cpp"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC idf::mbedtls
)
