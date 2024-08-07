#include "pch.hpp"

#include "render_pass.hpp"

#include "command_buffer.hpp"
#include "device.hpp"

namespace flwfrg::vk
{


RenderPass::RenderPass(Device *device, glm::vec4 draw_area, VkFormat color_format, VkFormat depth_format, glm::vec4 clear_color, float depth, uint32_t stencil)
	: device_{device},
	  draw_area_{draw_area},
	  clear_color_{clear_color},
	  depth{depth},
	  stencil{stencil},
	  state_{State::NOT_ALLOCATED}
{
	assert(device_ != nullptr);

	// Main subpass
	VkSubpassDescription main_subpass{};
	main_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	// Attachments
	std::vector<VkAttachmentDescription> attachment_descriptions;
	attachment_descriptions.resize(2);

	// Color attachment
	VkAttachmentDescription color_attachment{};
	color_attachment.format = color_format;
	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	color_attachment.flags = 0;

	attachment_descriptions[0] = color_attachment;

	VkAttachmentReference color_attachment_ref{};
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	main_subpass.colorAttachmentCount = 1;
	main_subpass.pColorAttachments = &color_attachment_ref;

	// Depth attachment
	VkAttachmentDescription depth_attachment{};
	depth_attachment.format = depth_format;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	attachment_descriptions[1] = depth_attachment;

	VkAttachmentReference depth_attachment_ref{};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	main_subpass.pDepthStencilAttachment = &depth_attachment_ref;

	// Input from shader
	main_subpass.inputAttachmentCount = 0;
	main_subpass.pInputAttachments = nullptr;

	// Resolve
	main_subpass.pResolveAttachments = nullptr;

	// Preserve
	main_subpass.preserveAttachmentCount = 0;
	main_subpass.pPreserveAttachments = nullptr;

	// Render pass dependencies
	VkSubpassDependency subpass_dependency{};
	subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpass_dependency.dstSubpass = 0;
	subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.srcAccessMask = 0;
	subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpass_dependency.dependencyFlags = 0;

	// Create info
	VkRenderPassCreateInfo render_pass_info{};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
	render_pass_info.pAttachments = attachment_descriptions.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &main_subpass;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &subpass_dependency;
	render_pass_info.pNext = nullptr;
	render_pass_info.flags = 0;

	// Create the render pass
	if (vkCreateRenderPass(device_->get_logical_device(), &render_pass_info, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass");
	}

	state_ = State::READY;
	FLOWFORGE_TRACE("Render pass created successfully");
}

RenderPass::~RenderPass()
{
	if (state_ != State::NOT_ALLOCATED)
	{
		vkDestroyRenderPass(device_->get_logical_device(), handle_, nullptr);
		FLOWFORGE_TRACE("Render pass destroyed");
	}
}

void RenderPass::set_render_area(glm::vec4 draw_area)
{
	draw_area_ = draw_area;
}

void RenderPass::begin(CommandBuffer &command_buffer, VkFramebuffer frame_buffer)
{
	// if (state_ != State::READY)
	// {
	// 	throw std::runtime_error("Render pass not ready to begin");
	// }

	VkRenderPassBeginInfo render_pass_begin_info{};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = handle_;
	render_pass_begin_info.framebuffer = frame_buffer;
	render_pass_begin_info.renderArea.offset = {static_cast<int32_t>(draw_area_.x), static_cast<int32_t>(draw_area_.y)};
	render_pass_begin_info.renderArea.extent = {static_cast<uint32_t>(draw_area_.z), static_cast<uint32_t>(draw_area_.w)};

	std::array<VkClearValue, 2> clear_values{};
	clear_values[0].color = {clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w};
	clear_values[1].depthStencil = {depth, stencil};

	render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
	render_pass_begin_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(command_buffer.handle_, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	command_buffer.state_ = CommandBuffer::State::IN_RENDER_PASS;

	state_ = State::IN_RENDER_PASS;
}

void RenderPass::end(CommandBuffer &command_buffer)
{
	if (state_ != State::IN_RENDER_PASS)
	{
		throw std::runtime_error("Render pass not in progress");
	}

	vkCmdEndRenderPass(command_buffer.handle_);

	command_buffer.state_ = CommandBuffer::State::RECORDING;

	state_ = State::RECORDING_ENDED;
}

}// namespace flwfrg::vk
