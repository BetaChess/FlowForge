cmake_minimum_required(VERSION 3.20)

project(flowforge_lib)

set(SOURCES
    pch.hpp
    ../include/flowforge.hpp
    logging/logger.hpp
    logging/logger.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_precompile_headers(${PROJECT_NAME}
        PUBLIC pch.hpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ../include/
        PUBLIC $ENV{VULKAN_SDK}/include/
        PUBLIC ../vendor/glfw/include/
        PUBLIC ../vendor/spdlog/include/
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ../vendor/glfw/src
)

target_link_libraries(${PROJECT_NAME}
        glfw
        ${Vulkan_LIBRARY}
)

include_directories(.)
