cmake_minimum_required(VERSION 3.20)

project(imgui_demo)

set(SOURCES
        main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ../../src/
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ../../src/
)

target_link_libraries(${PROJECT_NAME}
        flowforge_lib
)