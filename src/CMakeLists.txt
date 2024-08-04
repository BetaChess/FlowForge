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
        ../include/flowforge.hpp
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
