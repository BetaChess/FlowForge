cmake_minimum_required(VERSION 3.20)

project(texture_demo)

set(SOURCES
        main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${FLOWFORGELIB_PATH}/src/
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ${FLOWFORGELIB_PATH}/src/
)

target_link_libraries(${PROJECT_NAME}
        flowforge_lib
)


############## Build shaders ##############

add_shaders(${PROJECT_NAME}_shaders
        ${FLOWFORGELIB_PATH}/assets/shaders/default_material_shader.vert
        ${FLOWFORGELIB_PATH}/assets/shaders/default_material_shader.frag
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)