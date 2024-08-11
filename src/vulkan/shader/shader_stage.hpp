#pragma once

#include "vulkan/util/status_optional.hpp"
#include "vulkan/util/handle.hpp"

#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{
class Device;

constexpr const char *get_shader_stage_file_extension(VkShaderStageFlagBits stage)
{
	switch (stage)
	{
		case VK_SHADER_STAGE_VERTEX_BIT:
			return ".vert.spv";
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return ".frag.spv";
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return ".comp.spv";
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return ".geom.spv";
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return ".tesc.spv";
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return ".tese.spv";
		default:
			throw std::runtime_error("Unknown or invalid shader stage");
	}
}

class ShaderStage
{
public:
	ShaderStage() = default;
	~ShaderStage();

	// Copy
	ShaderStage(const ShaderStage &) = delete;
	ShaderStage &operator=(const ShaderStage &) = delete;
	// Move
	ShaderStage(ShaderStage &&other) noexcept = default;
	ShaderStage &operator=(ShaderStage &&other) noexcept = default;

	static StatusOptional<ShaderStage, Status, Status::SUCCESS> create_shader_module(
			Device *device,
			const std::string &name,
			VkShaderStageFlagBits shader_stage_flag);

	[[nodiscard]] inline VkPipelineShaderStageCreateInfo get_shader_stage_create_info() const { return shader_stage_create_info; }

private:
	Device *device_ = nullptr;

	VkShaderModuleCreateInfo create_info{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
	Handle<VkShaderModule> handle_{};
	VkPipelineShaderStageCreateInfo shader_stage_create_info{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
};

}// namespace flwfrg::vk