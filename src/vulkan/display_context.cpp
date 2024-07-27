#include "pch.hpp"

#include "display_context.hpp"

#include "command_buffer.hpp"

namespace flwfrg::vk
{

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
	swapchain_.frame_buffers_.clear();

	for (size_t i = 0; i < swapchain_.get_image_count(); i++)
	{
		std::vector<VkImageView> attachments{swapchain_.swapchain_image_views_[i], swapchain_.depth_attachment_->get_image_view()};
		swapchain_.frame_buffers_.emplace_back(
			&device_,
			&main_render_pass_,
			device_.get_swapchain_support_details().capabilities.currentExtent.width,
			device_.get_swapchain_support_details().capabilities.currentExtent.height,
			attachments);
	}
}

}// namespace flwfrg::vk