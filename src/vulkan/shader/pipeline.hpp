#pragma once

#include <vulkan/vulkan_core.h>

#include "vulkan/descriptor.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/util/status_optional.hpp"

namespace flwfrg::vk
{
class CommandBuffer;
class Device;

class Pipeline
{
public:
	struct PipelineConfig {
		const RenderPass *p_renderpass;
		const std::vector<VkVertexInputAttributeDescription> *p_attributes;
		const std::vector<VkDescriptorSetLayout> *p_descriptor_set_layouts;
		const std::vector<VkPipelineShaderStageCreateInfo> *p_stages;
		VkViewport viewport;
		VkRect2D scissor;
		uint32_t vertex_stride;
	};

public:
	Pipeline() = default;
	~Pipeline();

	// Copy
	Pipeline(const Pipeline &) = delete;
	Pipeline &operator=(const Pipeline &) = delete;
	// Move
	Pipeline(Pipeline &&other) noexcept = default;
	Pipeline &operator=(Pipeline &&other) noexcept = default;

	// methods

	[[nodiscard]] VkPipelineLayout layout() const { return pipeline_layout_; }

	[[nodiscard]] VkPipeline handle() const { return handle_; }

	void bind(CommandBuffer &command_buffer, VkPipelineBindPoint bind_point) const;

	// Static methods

	static StatusOptional<Pipeline, Status, Status::SUCCESS> create_pipeline(
			Device *device,
			PipelineConfig pipeline_config,
			bool is_wireframe);

private:
	Device *device_ = nullptr;

	Handle<VkPipeline> handle_{};
	Handle<VkPipelineLayout> pipeline_layout_{};
};

}// namespace flwfrg::vk