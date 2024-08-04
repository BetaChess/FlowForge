#pragma once

#include "util/handle.hpp"

#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{

class Device;
class RenderPass;

class FrameBuffer
{
public:
	FrameBuffer(Device *device, RenderPass *renderpass, uint32_t width, uint32_t height, std::vector<VkImageView> attachments);
	~FrameBuffer();

	// Copy
	FrameBuffer(const FrameBuffer &) = delete;
	FrameBuffer &operator=(const FrameBuffer &) = delete;
	// Move
	FrameBuffer(FrameBuffer &&other) noexcept = default;
	FrameBuffer &operator=(FrameBuffer &&other) noexcept = default;

	// Methods

	[[nodiscard]] VkFramebuffer handle() noexcept { return handle_; };

private:
	Device *device_;

	Handle<VkFramebuffer> handle_;
	RenderPass *renderpass_;
	std::vector<VkImageView> attachments_;
};

}