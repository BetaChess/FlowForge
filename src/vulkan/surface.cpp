#include "pch.hpp"

#include "instance.hpp"
#include "surface.hpp"
#include "window.hpp"

namespace flwfrg::vk
{

Surface::Surface(Instance const *instance, Window const *window)
	: instance_{instance}, window_{window}, surface_{window->create_window_surface(instance->handle())}
{
}
Surface::~Surface()
{
	if (surface_.not_null())
	{
		vkDestroySurfaceKHR(instance_->handle(), surface_, nullptr);
		FLOWFORGE_INFO("Vulkan surface destroyed");
	}
};

}// namespace flwfrg::vk