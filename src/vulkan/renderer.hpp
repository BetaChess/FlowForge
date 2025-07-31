#pragma once

#include "display_context.hpp"
#include "glfw_context.hpp"
#include "util/status_optional.hpp"
#include "window.hpp"

namespace flwfrg::vk
{

class Renderer
{
public:
	enum class RendererStatus
	{
		SUCCESS,
		SWAPCHAIN_RESIZE,
		WINDOW_SHOULD_CLOSE,
		FAILED_TO_WAIT_ON_FENCE,
		UNKNOWN_ERROR
	};

public:
	Renderer(uint32_t initial_width, uint32_t initial_height, std::string window_name, PhysicalDeviceRequirements requirements = {});
	~Renderer() = default;

	// Copy
	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;
	// Move
	Renderer(Renderer &&other) noexcept = delete;
	Renderer &operator=(Renderer &&other) noexcept = delete;

	// Methods

	[[nodiscard]] inline uint64_t get_frame() const { return display_context_.get_current_frame(); };
	[[nodiscard]] inline bool should_close() const { return window_.should_close(); };
	[[nodiscard]] inline DisplayContext &get_display_context() { return display_context_; };
	[[nodiscard]] inline Window &get_window() { return window_; };

	StatusOptional<CommandBuffer *, RendererStatus, RendererStatus::SUCCESS> begin_frame();
	RendererStatus end_frame();

private:
	std::string window_name_;

	GLFWContext glfw_context_{};
	Window window_;
	DisplayContext display_context_;
};

}// namespace flwfrg::vk