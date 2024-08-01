#include "pch.hpp"

#include "shader_stage.hpp"

#include "vulkan/device.hpp"

#include <fstream>

namespace flwfrg::vk
{


ShaderStage::~ShaderStage()
{
	if (handle_.not_null())
	{
		vkDestroyShaderModule(device_->get_logical_device(), handle_, nullptr);
	}
}

StatusOptional<ShaderStage, ShaderStage::ShaderStageCreationStatus, ShaderStage::ShaderStageCreationStatus::SUCCESS> ShaderStage::create_shader_module(Device *device, const std::string &name, VkShaderStageFlagBits shader_stage_flag)
{
	assert(device != nullptr);

	std::string file_name{};
	try
	{
		file_name = "assets/shaders/" + name + get_shader_stage_file_extension(shader_stage_flag);
	} catch (const std::runtime_error &e)
	{
		FLOWFORGE_ERROR(e.what());
		FLOWFORGE_ERROR("Shader stage flag was {}", shader_stage_flag);
		return ShaderStageCreationStatus::UNKNOWN_OR_INVALID_SHADER_STAGE;
	}

	ShaderStage return_stage;
	return_stage.device_ = device;

	// Read in the file
	std::ifstream file(file_name, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		FLOWFORGE_ERROR("Failed to open file: {}", file_name);
		return ShaderStageCreationStatus::FAILED_TO_OPEN_FILE;
	}

	std::vector<uint8_t> file_buffer;
	// Read the entire file as binary
	file.seekg(0, std::ios::end);
	file_buffer.resize(file.tellg());
	file.seekg(0);
	file.read(reinterpret_cast<char *>(file_buffer.data()),
			  static_cast<long>(file_buffer.size()));

	// Set shader stage info
	return_stage.create_info.codeSize = file_buffer.size();
	return_stage.create_info.pCode = reinterpret_cast<const uint32_t *>(file_buffer.data());

	// Close the file
	file.close();

	// Create the shader module and check the result
	auto result = vkCreateShaderModule(return_stage.device_->get_logical_device(), &return_stage.create_info, nullptr, return_stage.handle_.ptr());
	if (result != VK_SUCCESS)
	{
		FLOWFORGE_ERROR("Failed to create shader module");
		switch (result)
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				return {ShaderStageCreationStatus::OUT_OF_HOST_MEMORY};
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return {ShaderStageCreationStatus::OUT_OF_DEVICE_MEMORY};
			case VK_ERROR_INVALID_SHADER_NV:
				return  {ShaderStageCreationStatus::INVALID_SHADER};
			default:
				return ShaderStageCreationStatus::UNKNOWN_ERROR;
		}
	}

	return_stage.create_info.pCode = nullptr;

	// Set shader stage create info
	return_stage.shader_stage_create_info.stage = shader_stage_flag;
	return_stage.shader_stage_create_info.module = return_stage.handle_;
	return_stage.shader_stage_create_info.pName = "main";// Shader entry point

	return return_stage;
}

}// namespace flwfrg::vk