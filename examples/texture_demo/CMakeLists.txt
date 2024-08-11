cmake_minimum_required(VERSION 3.20)

project(texture_demo)

set(SOURCES
        main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(FLOWFORGEROOT ../..)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${FLOWFORGEROOT}/src/
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ${FLOWFORGEROOT}/src/
)

target_link_libraries(${PROJECT_NAME}
        flowforge_lib
)

add_custom_target(
        ${PROJECT_NAME}_shaders
        DEPENDS ${flowforge_lib_shaders}
)

add_custom_command(
        TARGET ${PROJECT_NAME}_shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/${FLOWFORGEROOT}/assets/shaders ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders
        COMMENT "Copying shaders" VERBATIM
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)