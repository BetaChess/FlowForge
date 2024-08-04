#pragma once

#include "util/handle.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{
class Device;
class CommandBuffer;


class RenderPass
{
public:
	enum class State
	{
		READY,
		RECORDING,
		IN_RENDER_PASS,
		RECORDING_ENDED,
		SUBMITTED,
		NOT_ALLOCATED
	};

public:
	RenderPass(Device *device, glm::vec4 draw_area, VkFormat color_format, VkFormat depth_format, glm::vec4 clear_color, float depth, uint32_t stencil);
	~RenderPass();

	// Not copyable or movable
	RenderPass(const RenderPass &) = delete;
	RenderPass &operator=(const RenderPass &) = delete;
	RenderPass(RenderPass &&) = delete;
	RenderPass &operator=(RenderPass &&) = delete;

	// methods

	[[nodiscard]] constexpr VkRenderPass handle() const { return handle_; };

	void set_render_area(glm::vec4 draw_area);

	void begin(CommandBuffer &command_buffer, VkFramebuffer frame_buffer);
	void end(CommandBuffer &command_buffer);

private:
	Device *device_;

	Handle<VkRenderPass> handle_;
	glm::vec4 draw_area_;
	glm::vec4 clear_color_;

	float depth;
	uint32_t stencil;

	State state_;
};

}// namespace flwfrg::vk