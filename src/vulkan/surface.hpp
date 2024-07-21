#pragma once

#include <vulkan/vulkan_core.h>

#include "handle.hpp"

namespace flwfrg::vk
{
class Instance;
class Window;

class Surface
{
public:
	Surface(const Instance *const instance, const Window *const window);
	~Surface();

	[[nodiscard]] constexpr VkSurfaceKHR handle() const noexcept { return surface_; };

private:
	const Instance *const instance_;
	const Window *const window_;
	VkSurfaceKHR surface_;
};

}// namespace flwfrg::vk