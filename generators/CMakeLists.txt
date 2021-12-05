project(luna.generators)

add_library(${PROJECT_NAME}
    "ConstantGenerator.cpp"
    "FlameGenerator.cpp"
    "InterpolatingGenerator.cpp"
    "Noise.cpp"
    "PlasmaGenerator.cpp"
    "EffectMixer.cpp"
    "ConstantEffect.cpp"
    "FlameEffect.cpp"
    "PlasmaEffect.cpp"
    "EffectPlugin.cpp"
    "EffectSet.cpp"
)

target_include_directories(${PROJECT_NAME}
    INTERFACE "include"
    PRIVATE "include/luna"
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC luna.core Eigen3::Eigen prism idf::freertos
)
