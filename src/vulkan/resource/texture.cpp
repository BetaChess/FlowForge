#include "pch.hpp"

#include "texture.hpp"

#include "vulkan/buffer.hpp"
#include "vulkan/command_buffer.hpp"
#include "vulkan/device.hpp"

#include <stb_image.h>

namespace flwfrg::vk
{

Texture::~Texture()
{
	if (sampler_.not_null())
	{
		vkDestroySampler(device_->get_logical_device(), sampler_, nullptr);
	}
}

Status Texture::load_texture_from_file(std::string texture_name)
{
	std::string path = "assets/textures/" + texture_name + ".png";
	const int32_t required_channel_count = 4;
	stbi_set_flip_vertically_on_load(true);

	int32_t width, height, channel_count;

	uint8_t *data = stbi_load(path.c_str(),
							  &width,
							  &height,
							  &channel_count,
							  required_channel_count);


	if (stbi_failure_reason())
	{
		FLOWFORGE_WARN("Failed to load texture '{}', {}", path, stbi_failure_reason());
	}

	if (data == nullptr)
	{
		return Status::FLOWFORGE_FAILED_TO_LOAD_TEXTURE_DATA;
	}

	uint32_t generation = generation_;
	generation_ = constant::invalid_generation;
	uint64_t total_size = width * height * required_channel_count;

	bool has_transparency = false;
	for (size_t i = 0; i < total_size; i += 4)
	{
		uint8_t a = data[i + 3];
		if (a < 255)
		{
			has_transparency = true;
			break;
		}
	}

	std::vector<uint8_t> data_vec;
	data_vec.resize(total_size);
	std::copy_n(data, total_size, data_vec.begin());
	// for (size_t i = 0; i < total_size; i += 4)
	// {
	// 	data_vec[i / 4 + 0] = data[i] / 255.0f;
	// 	data_vec[i / 4 + 1] = data[i + 1] / 255.0f;
	// 	data_vec[i / 4 + 2] = data[i + 2] / 255.0f;
	// 	data_vec[i / 4 + 3] = data[i + 3] / 255.0f;
	// }
	stbi_image_free(data);

	{
		auto opt_status = create_texture(device_, id_, static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint8_t>(generation), has_transparency, data_vec);
		if (!opt_status.has_value())
			return opt_status.status();

		*this = std::move(opt_status.value());
	}

	if (generation == constant::invalid_generation)
		generation_ = 0;
	else
		generation_ = generation + 1;

	return Status::SUCCESS;
}

StatusOptional<Texture, Status, Status::SUCCESS> Texture::create_texture(Device *device, uint32_t id, uint32_t width, uint32_t height, uint8_t channel_count, bool has_transparency, std::vector<uint8_t> data)
{
	assert(device != nullptr);

	Texture return_texture;
	return_texture.device_ = device;
	return_texture.id_ = id;
	return_texture.width_ = width;
	return_texture.height_ = height;
	return_texture.channel_count_ = channel_count;
	return_texture.has_transparency_ = has_transparency;
	return_texture.data_ = std::move(data);

	VkDeviceSize image_size = return_texture.width_ * return_texture.height_ * return_texture.channel_count_;


	assert(image_size == return_texture.data_.size());

	VkFormat image_format;
	switch (return_texture.channel_count_)
	{
		case 1:
			image_format = VK_FORMAT_R8_UINT;
			break;
		case 2:
			image_format = VK_FORMAT_R8G8_UINT;
			break;
		case 3:
			image_format = VK_FORMAT_R8G8B8_UINT;
			break;
		case 4:
			image_format = VK_FORMAT_R8G8B8A8_UINT;
			break;
		default:
			return Status::FLOWFORGE_UNSUPPORTED_CHANNEL_COUNT;
	}

	// Create staging buffer
	VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	Buffer staging_buffer{return_texture.device_, image_size, usage, memory_flags, true};

	staging_buffer.load_data(return_texture.data_.data(), 0, image_size, 0);

	return_texture.image_ = Image(return_texture.device_,
								  return_texture.width_, return_texture.height_,
								  image_format,
								  VK_IMAGE_TILING_OPTIMAL,
								  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
								  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
								  VK_IMAGE_ASPECT_COLOR_BIT,
								  true);

	VkCommandPool pool = return_texture.device_->get_graphics_command_pool();
	VkQueue queue = return_texture.device_->get_graphics_queue();
	CommandBuffer temp_buffer = CommandBuffer::begin_single_time_commands(return_texture.device_, pool);

	// Transition the layout to the optimal for recieving data
	return_texture.image_.transition_layout(temp_buffer, image_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy data from the buffer
	return_texture.image_.copy_from_buffer(temp_buffer, staging_buffer);

	// Transition to optimal read layout
	return_texture.image_.transition_layout(temp_buffer, image_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	CommandBuffer::end_single_time_commands(return_texture.device_, temp_buffer, queue);

	// Create sampler
	VkSamplerCreateInfo sampler_info{};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = VK_FILTER_LINEAR;
	sampler_info.minFilter = VK_FILTER_LINEAR;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = VK_TRUE;
	sampler_info.maxAnisotropy = 16;
	sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_info.unnormalizedCoordinates = VK_FALSE;
	sampler_info.compareEnable = VK_FALSE;
	sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_info.mipLodBias = 0.0f;
	sampler_info.minLod = 0.0f;
	sampler_info.maxLod = 0.0f;

	// Create the sampler and check the result
	auto result = vkCreateSampler(return_texture.device_->get_logical_device(), &sampler_info, nullptr, return_texture.sampler_.ptr());
	if (result != VK_SUCCESS)
	{
		switch (result)
		{
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				return {Status::OUT_OF_HOST_MEMORY};
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return {Status::OUT_OF_DEVICE_MEMORY};
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR:
				return {Status::INVALID_OPAQUE_CAPTURE_ADDRESS_KHR};
			default:
				return {Status::UNKNOWN_ERROR};
		}
	}

	return_texture.generation_++;

	return return_texture;
}
}// namespace flwfrg::vk