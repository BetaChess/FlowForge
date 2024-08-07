#pragma once

#include "../src/logging/logger.hpp"
#include "../src/vulkan/instance.hpp"
#include "../src/vulkan/device.hpp"
#include "../src/vulkan/window.hpp"
#include "../src/vulkan/surface.hpp"
#include "../src/glfw_context.hpp"
#include "../src/vulkan/display_context.hpp"
#include "../src/vulkan/shader/default/imgui_shader.hpp"
#include "../src/vulkan/renderer.hpp"

namespace flwfrg
{

inline void init()
{
	Logger::init();
}

}// namespace flwfrg