cmake_minimum_required(VERSION 3.20)

project(simple_square)

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
        PUBLIC ../../include
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ../../src/
)

target_link_libraries(${PROJECT_NAME}
        flowforge_lib
)