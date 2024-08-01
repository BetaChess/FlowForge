#pragma once

#include <vulkan/vulkan_core.h>

#include "../util/status_optional.hpp"
#include "vulkan/descriptor.hpp"
#include "vulkan/render_pass.hpp"

namespace flwfrg::vk
{
class CommandBuffer;
class Device;

class Pipeline
{
public:
	enum class PipelineCreationStatus
	{
		SUCCESS,
		OUT_OF_HOST_MEMORY,
		OUT_OF_DEVICE_MEMORY,
		INVALID_SHADER,
		UNKNOWN_ERROR,
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

	static StatusOptional<Pipeline, PipelineCreationStatus, PipelineCreationStatus::SUCCESS> create_pipeline(
			Device *device,
			const RenderPass &renderpass,
			const std::vector<VkVertexInputAttributeDescription> &attributes,
			const std::vector<VkDescriptorSetLayout> &descriptor_set_layouts,
			const std::vector<VkPipelineShaderStageCreateInfo> &stages,
			VkViewport viewport,
			VkRect2D scissor,
			bool is_wireframe);

private:
	Device *device_ = nullptr;

	Handle<VkPipeline> handle_{};
	Handle<VkPipelineLayout> pipeline_layout_{};
};

}// namespace flwfrg::vk