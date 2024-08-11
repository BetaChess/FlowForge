cmake_minimum_required(VERSION 3.20)

project(simple_square)

set(SOURCES
        main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

set(FLOWFORGELIB_PATH ../..)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        CXX_STANDARD 20
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


############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        ${VULKAN_SDK_PATH}/Bin
        ${VULKAN_SDK_PATH}/Bin32
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)

function(add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN}) # the rest of arguments to this function will be assigned as shader source files

    # Validate that source files have been passed
    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if (FILE_COUNT EQUAL 0)
        message(FATAL_ERROR "Cannot create a shaders target without any source files")
    endif ()

    set(SHADER_COMMANDS)
    set(SHADER_PRODUCTS)

    foreach (SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

        # Build command
        list(APPEND SHADER_COMMANDS COMMAND)
        list(APPEND SHADER_COMMANDS ${GLSL_VALIDATOR})
        list(APPEND SHADER_COMMANDS "-V")
        list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
        list(APPEND SHADER_COMMANDS "-o")
        list(APPEND SHADER_COMMANDS "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${SHADER_NAME}.spv")

        # Add product
        list(APPEND SHADER_PRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${SHADER_NAME}.spv")

    endforeach ()

    add_custom_target(${TARGET_NAME} ALL
            ${SHADER_COMMANDS}
            COMMENT "Compiling Shaders [${TARGET_NAME}]"
            SOURCES ${SHADER_SOURCE_FILES}
            BYPRODUCTS ${SHADER_PRODUCTS}
    )
endfunction()

add_shaders(${PROJECT_NAME}_shaders
        ${FLOWFORGELIB_PATH}/assets/shaders/default_simple_shader.vert
        ${FLOWFORGELIB_PATH}/assets/shaders/default_simple_shader.frag
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
