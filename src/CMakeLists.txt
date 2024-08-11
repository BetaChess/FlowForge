cmake_minimum_required(VERSION 3.20)

project(flowforge_lib)

## Compile C libary
set(STB_INTERNAL_LIB_NAME stb_internal)

add_library(${STB_INTERNAL_LIB_NAME} STATIC stb_image.c
)

target_include_directories(${STB_INTERNAL_LIB_NAME}
        PUBLIC ../vendor/stb
)

set(SOURCES
        # Dear ImGui sources
        ${IMGUI_SOURCES}
        #Project sources
        pch.hpp
        flowforge.hpp
        logging/logger.hpp
        logging/logger.cpp
        glfw_context.hpp
        glfw_context.cpp
        vulkan/instance.hpp
        vulkan/util/handle.hpp
        vulkan/instance.cpp
        vulkan/device.hpp
        vulkan/device.cpp
        vulkan/surface.hpp
        vulkan/surface.cpp
        vulkan/window.hpp
        vulkan/window.cpp
        vulkan/swapchain.hpp
        vulkan/swapchain.cpp
        vulkan/image.hpp
        vulkan/image.cpp
        vulkan/buffer.hpp
        vulkan/buffer.cpp
        vulkan/command_buffer.hpp
        vulkan/command_buffer.cpp
        vulkan/frame_buffer.hpp
        vulkan/frame_buffer.cpp
        vulkan/render_pass.hpp
        vulkan/render_pass.cpp
        vulkan/display_context.hpp
        vulkan/display_context.cpp
        vulkan/fence.hpp
        vulkan/fence.cpp
        vulkan/descriptor.hpp
        vulkan/descriptor.cpp
        vulkan/shader/pipeline.hpp
        vulkan/shader/pipeline.cpp
        vulkan/util/status_optional.hpp
        vulkan/shader/vertex.hpp
        vulkan/shader/shader_stage.hpp
        vulkan/shader/shader_stage.cpp
        vulkan/resource/texture.hpp
        vulkan/resource/texture.cpp
        vulkan/util/constants.hpp
        vulkan/shader/default/imgui_shader.hpp
        vulkan/shader/default/imgui_shader.cpp
        vulkan/renderer.hpp
        vulkan/renderer.cpp
        vulkan/debug_messenger.hpp
        vulkan/debug_messenger.cpp
        vulkan/shader/default/material_shader.hpp
        vulkan/shader/default/material_shader.cpp
        vulkan/shader/default/material_object_types.inl
        default_shaders.hpp
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
        PUBLIC ${IMGUI_DIR}/backends/
        PUBLIC ${IMGUI_DIR}
        PUBLIC ../vendor/stb
        PUBLIC ../vendor/glm
)

target_link_directories(${PROJECT_NAME}
        PRIVATE ../vendor/glfw/src
)

target_link_libraries(${PROJECT_NAME}
        glfw
        ${Vulkan_LIBRARY}
        ${STB_INTERNAL_LIB_NAME}
)

include_directories(.)

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

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "../assets/shaders/*.frag"
        "../assets/shaders/*.vert"
)

message("${GLSL_SOURCE_FILES}")

foreach (GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/../assets/shaders/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach (GLSL)

add_custom_target(
        ${PROJECT_NAME}_shaders
        SOURCES ${GLSL_SOURCE_FILES}
        DEPENDS ${SPIRV_BINARY_FILES}
)

add_custom_command(
        TARGET ${PROJECT_NAME}_shaders POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/../assets/shaders ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders
        COMMENT "Copying shaders" VERBATIM
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_shaders)
