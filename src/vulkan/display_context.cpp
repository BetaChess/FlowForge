#include "pch.hpp"

#include "display_context.hpp"

#include "command_buffer.hpp"

namespace flwfrg::vk
{
DisplayContext::DisplayContext(Window *window, PhysicalDeviceRequirements requirements, bool enable_validation_layers)
	: window_{window},
	  instance_{enable_validation_layers},
	  debug_messenger_(&instance_),
	  surface_{&instance_, window_},
	  device_{&instance_, &surface_, requirements},
	  swapchain_{this}
{
	FLOWFORGE_INFO("Creating frame buffers");
	swapchain_.regenerate_frame_buffers(&main_render_pass_);
	FLOWFORGE_INFO("Creating command buffers");
	create_command_buffers();

	image_avaliable_semaphores_.resize(swapchain_.max_frames_in_flight_);
	queue_complete_semaphores_.resize(swapchain_.max_frames_in_flight_);
	for (size_t i = 0; i < swapchain_.max_frames_in_flight_; i++)
	{
		VkSemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(device_.get_logical_device(), &semaphore_info, nullptr, &image_avaliable_semaphores_[i]);
		vkCreateSemaphore(device_.get_logical_device(), &semaphore_info, nullptr, &queue_complete_semaphores_[i]);

		in_flight_fences_.emplace_back(&device_, true);
	}

	images_in_flight_.resize(swapchain_.get_image_count());
}
DisplayContext::~DisplayContext()
{
	if (device_.get_logical_device() == VK_NULL_HANDLE)
		return;

	vkDeviceWaitIdle(device_.get_logical_device());

	// Destroy semaphores
	for (size_t i = 0; i < swapchain_.max_frames_in_flight_; i++)
	{
		vkDestroySemaphore(device_.get_logical_device(), image_avaliable_semaphores_[i], nullptr);
		vkDestroySemaphore(device_.get_logical_device(), queue_complete_semaphores_[i], nullptr);
	}
}

void DisplayContext::create_command_buffers()
{
	graphics_command_buffers_.clear();

	for (size_t i = 0; i < swapchain_.get_image_count(); i++)
	{
		graphics_command_buffers_.emplace_back(&device_, device_.get_graphics_command_pool(), true);
	}
}

void DisplayContext::regenerate_frame_buffers()
{
	swapchain_.regenerate_frame_buffers(&main_render_pass_);
}

}// namespace flwfrg::vk