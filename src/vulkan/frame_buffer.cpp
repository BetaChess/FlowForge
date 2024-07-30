#include "pch.hpp"

#include "frame_buffer.hpp"

#include "device.hpp"
#include "render_pass.hpp"

namespace flwfrg::vk
{


FrameBuffer::FrameBuffer(Device *device, RenderPass *renderpass, uint32_t width, uint32_t height, std::vector<VkImageView> attachments)
	: device_{device},
	  renderpass_{renderpass},
	  attachments_{std::move(attachments)}
{
	assert(device_ != nullptr);

	VkFramebufferCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	create_info.renderPass = renderpass->get_handle();
	create_info.attachmentCount = static_cast<uint32_t>(attachments_.size());
	create_info.pAttachments = attachments_.data();
	create_info.width = width;
	create_info.height = height;
	create_info.layers = 1;

	if (vkCreateFramebuffer(device_->get_logical_device(), &create_info, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create framebuffer");
	}
	FLOWFORGE_TRACE("Framebuffer created");
}

FrameBuffer::~FrameBuffer()
{
	if (handle_.not_null())
	{
		vkDestroyFramebuffer(device_->get_logical_device(), handle_, nullptr);
		FLOWFORGE_TRACE("Framebuffer destroyed");
	}
}
}// namespace flwfrg::vk