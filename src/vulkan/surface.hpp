#pragma once

#include <vulkan/vulkan_core.h>

#include "util/handle.hpp"

namespace flwfrg::vk
{
class Instance;
class Window;

class Surface
{
public:
	Surface(Instance const *instance, Window const *window);
	~Surface();

	// Copy
	Surface(const Surface &) = delete;
	Surface &operator=(const Surface &) = delete;
	// Move
	Surface(Surface &&other) noexcept = default;
	Surface &operator=(Surface &&other) noexcept = default;

	[[nodiscard]] constexpr VkSurfaceKHR handle() const noexcept { return surface_; };

private:
	Instance const *instance_;
	Window const *window_;
	Handle<VkSurfaceKHR> surface_;
};

}// namespace flwfrg::vk