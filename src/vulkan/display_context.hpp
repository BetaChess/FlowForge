#pragma once

#include "command_buffer.hpp"
#include "device.hpp"
#include "fence.hpp"
#include "instance.hpp"
#include "render_pass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "window.hpp"

namespace flwfrg::vk
{

class DisplayContext
{
public:
	explicit DisplayContext(Window* window, bool enable_validation_layers = true);
	~DisplayContext();

	// Copy
	DisplayContext(const DisplayContext &) = delete;
	DisplayContext &operator=(const DisplayContext &) = delete;
	// Move
	DisplayContext(DisplayContext &&other) noexcept = delete;
	DisplayContext &operator=(DisplayContext &&other) noexcept = delete;

private:
	Window* window_ = nullptr;
	Instance instance_;
	Surface surface_;

	Device device_;
	Swapchain swapchain_;

	RenderPass main_render_pass_{
		&device_,
		{0, 0, window_->get_width(), window_->get_height()},
		swapchain_.swapchain_image_format_.format,
		device_.get_depth_format(),
		{0, 0, 0.2f, 1.0f},
		1.0f,
		0};

	std::vector<CommandBuffer> graphics_command_buffers_{};

	std::vector<VkSemaphore> image_avaliable_semaphores_;
	std::vector<VkSemaphore> queue_complete_semaphores_;

	std::vector<Fence> in_flight_fences_;
	std::vector<Fence*> images_in_flight_;

	// Current swapchain image index (next index may be unpredictable)
	uint32_t image_index_ = 0;
	// Current frame (will always change in a round-robin fashion).
	uint32_t current_frame_ = 0;

	// Methods

	void create_command_buffers();
	void regenerate_frame_buffers();

	friend Swapchain;
};

}
