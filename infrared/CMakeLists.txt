project(luna.infrared)

add_library(${PROJECT_NAME}
    "Infrared.cpp"
    "Ir40ButtonRemote.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core luna.generators
    PRIVATE idf::freertos
)
