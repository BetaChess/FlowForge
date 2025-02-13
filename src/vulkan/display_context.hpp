#pragma once

#include "command_buffer.hpp"
#include "debug_messenger.hpp"
#include "device.hpp"
#include "fence.hpp"
#include "instance.hpp"
#include "render_pass.hpp"
#include "surface.hpp"
#include "swapchain.hpp"
#include "window.hpp"

namespace flwfrg::vk
{
class Renderer;

class DisplayContext
{
public:
	explicit DisplayContext(Window *window, bool enable_validation_layers = true);
	~DisplayContext();

	// Copy
	DisplayContext(const DisplayContext &) = delete;
	DisplayContext &operator=(const DisplayContext &) = delete;
	// Move
	DisplayContext(DisplayContext &&other) noexcept = delete;
	DisplayContext &operator=(DisplayContext &&other) noexcept = delete;

	// Methods

	// Getters
	[[nodiscard]] inline Window *get_window() const { return window_; }
	[[nodiscard]] inline Instance &get_instance() { return instance_; }
	[[nodiscard]] inline Surface &get_surface() { return surface_; }
	[[nodiscard]] inline Device &get_device() { return device_; }
	[[nodiscard]] inline Swapchain &get_swapchain() { return swapchain_; }
	[[nodiscard]] inline RenderPass &get_main_render_pass() { return main_render_pass_; }
	[[nodiscard]] inline uint32_t get_frame_counter() const { return frame_counter; }
	[[nodiscard]] inline uint32_t get_image_index() const { return image_index_; }
	[[nodiscard]] inline uint32_t get_current_frame() const { return current_frame_; }

	inline CommandBuffer &get_command_buffer() { return graphics_command_buffers_[image_index_]; };
	inline Fence &get_current_frame_fence_in_flight() { return in_flight_fences_[current_frame_]; };
	inline Fence *get_image_index_frame_fence_in_flight() { return images_in_flight_[image_index_]; };
	inline VkFramebuffer get_frame_buffer_handle() { return swapchain_.frame_buffers_[image_index_].handle(); };

private:
	Window *window_ = nullptr;
	Instance instance_;
	DebugMessenger debug_messenger_;
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
	std::vector<Fence *> images_in_flight_;

	uint64_t frame_counter = 0;
	// Current swapchain image index (next index may be unpredictable)
	uint32_t image_index_ = 0;
	// Current frame (will always change in a round-robin fashion).
	uint32_t current_frame_ = 0;

	// Methods

	void create_command_buffers();
	void regenerate_frame_buffers();

	friend Swapchain;
	friend Renderer;
};

}// namespace flwfrg::vk
