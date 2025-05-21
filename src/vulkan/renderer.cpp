#include "pch.hpp"

#include "renderer.hpp"

#include <iostream>
#include <utility>

namespace flwfrg::vk
{


Renderer::Renderer(uint32_t initial_width, uint32_t initial_height, std::string window_name)
	: window_name_{std::move(window_name)}, window_{initial_width, initial_height, window_name_}, display_context_{&window_, true}
{
}

StatusOptional<CommandBuffer *, Renderer::RendererStatus, Renderer::RendererStatus::SUCCESS> Renderer::begin_frame()
{
	glfwPollEvents();
	if (window_.should_close())
		return RendererStatus::WINDOW_SHOULD_CLOSE;

	// Wait for the fence of the frame we wish to write to.
	if (!display_context_.get_current_frame_fence_in_flight().wait(std::numeric_limits<uint64_t>::max()))
	{
		FLOWFORGE_WARN("Failure to wait for fence in flight");
		return RendererStatus::FAILED_TO_WAIT_ON_FENCE;
	}
	// Get the next image index
	auto result = display_context_.swapchain_.acquire_next_image(
			std::numeric_limits<uint64_t>::max(),
			display_context_.image_avaliable_semaphores_[display_context_.current_frame_],
			VK_NULL_HANDLE,
			&display_context_.image_index_);
	if (result != Status::SUCCESS)
	{
		if (result == Status::OUT_OF_DATE_KHR)
			return RendererStatus::SWAPCHAIN_RESIZE;
		else
		{
			FLOWFORGE_ERROR("Failed to acquire next image");
			return RendererStatus::UNKNOWN_ERROR;
		}
	}

	CommandBuffer &command_buffer = display_context_.graphics_command_buffers_[display_context_.current_frame_];

	command_buffer.reset();
	command_buffer.begin(false, false, false);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(window_.get_width());
	viewport.height = static_cast<float>(window_.get_height());
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = {window_.get_width(), window_.get_height()};

	vkCmdSetViewport(command_buffer.get_handle(), 0, 1, &viewport);
	vkCmdSetScissor(command_buffer.get_handle(), 0, 1, &scissor);

	display_context_.main_render_pass_.set_render_area({0,0, display_context_.get_swapchain().get_frame_buffer_size()});
	// display_context_.main_render_pass_.set_render_area({0, 0, window_.get_width(), window_.get_height()});

	// Begin the render pass.
	display_context_.main_render_pass_.begin(command_buffer, display_context_.get_frame_buffer_handle());

	return &command_buffer;
}

Renderer::RendererStatus Renderer::end_frame()
{
	CommandBuffer &command_buffer = display_context_.graphics_command_buffers_[display_context_.current_frame_];

	display_context_.main_render_pass_.end(command_buffer);

	command_buffer.end();

	// Wait for the previous frame to not use the image
	// if (auto *fence = display_context_.get_image_index_frame_fence_in_flight())
	// {
	// 	if (!fence->wait(std::numeric_limits<uint64_t>::max()))
	// 	{
	// 		FLOWFORGE_WARN("Failed to wait for image index fence in flight");
	// 		return RendererStatus::FAILED_TO_WAIT_ON_FENCE;
	// 	}
	// }

	// Set the fence as image in flight
	display_context_.images_in_flight_[display_context_.image_index_] = &display_context_.get_current_frame_fence_in_flight();

	// Reset the fence
	display_context_.get_current_frame_fence_in_flight().reset();

	// Submit the queue
	VkPipelineStageFlags flags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	command_buffer.submit(
			display_context_.device_.get_graphics_queue(),
			display_context_.image_avaliable_semaphores_[display_context_.current_frame_],
			display_context_.queue_complete_semaphores_[display_context_.current_frame_],
			display_context_.get_current_frame_fence_in_flight().get_handle(),
			flags);

	// Give the image back to the swapchain
	auto result = display_context_.swapchain_.present(
			display_context_.device_.get_graphics_queue(),
			display_context_.device_.get_present_queue(),
			display_context_.queue_complete_semaphores_[display_context_.current_frame_],
			display_context_.image_index_);

	if (result != Status::SUCCESS)
	{
		if (result == Status::OUT_OF_DATE_KHR || result == Status::SUBOPTIMAL_KHR)
		{
			return RendererStatus::SWAPCHAIN_RESIZE;
		} else
		{
			FLOWFORGE_ERROR("Failed to present swap chain image");
			return RendererStatus::UNKNOWN_ERROR;
		}
	}

	return RendererStatus::SUCCESS;
}

}// namespace flwfrg::vk