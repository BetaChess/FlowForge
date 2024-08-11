#pragma once

#include "glfw_context.hpp"
#include "logging/logger.hpp"
#include "vulkan/device.hpp"
#include "vulkan/display_context.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/renderer.hpp"
#include "vulkan/surface.hpp"
#include "vulkan/window.hpp"

namespace flwfrg
{

inline void init()
{
	Logger::init();
}

}// namespace flwfrg