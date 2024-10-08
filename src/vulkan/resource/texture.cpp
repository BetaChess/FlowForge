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
	vkDeviceWaitIdle(device_->get_logical_device());

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
	return_texture.generation_ = constant::invalid_generation;

	VkDeviceSize image_size = return_texture.width_ * return_texture.height_ * return_texture.channel_count_;
	assert(image_size == return_texture.data_.size());

	auto image_format = compute_format(return_texture.channel_count_);
	if (image_format.status() != Status::SUCCESS)
		return image_format.status();

	// Create staging buffer
	VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	Buffer staging_buffer{return_texture.device_, image_size, usage, memory_flags, true};

	return_texture.image_ = Image(return_texture.device_,
									  return_texture.width_, return_texture.height_,
									  image_format.value(),
									  VK_IMAGE_TILING_OPTIMAL,
									  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
									  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
									  VK_IMAGE_ASPECT_COLOR_BIT,
									  true);

	return_texture.flush_data(staging_buffer, image_size, image_format.value());

	// Create sampler
	{
		auto sampler = create_sampler(return_texture.device_);
		if (sampler.status() != Status::SUCCESS)
			return sampler.status();

		return_texture.sampler_ = std::move(sampler.value());
	}

	return_texture.generation_ = 0;

	return return_texture;
}

StatusOptional<Texture, Status, Status::SUCCESS> Texture::generate_default_texture(Device *device)
{
	// Create a 256 by 256 default texture
	constexpr uint32_t texture_width = 256;
	constexpr uint32_t texture_height = 256;

	std::vector<uint8_t> texture_data;
	constexpr uint8_t channel_count = 4;
	texture_data.resize(texture_width * texture_height * channel_count);

	// constexpr Texture::Data purple = {.color = {200.f / 255.0f, 0.f, 200.f / 255.0f, 1}};
	// constexpr Texture::Data black = {.color = {0, 0, 0, 1.f}};

	// constexpr glm::vec4 purple = {200.0f / 255.0f, 0, 200.0f / 255.0f, 255.0f / 255.0f};
	// constexpr glm::vec4 black = {0, 0, 0, 255.0f / 255.0f};
	constexpr glm::ivec4 purple = {200, 0, 200, 255};
	constexpr glm::ivec4 black = {0, 0, 0, 255};

	for (size_t y = 0; y < texture_height; y++)
	{
		const uint32_t ysector = static_cast<uint32_t>(y) / 8;
		for (size_t x = 0; x < texture_width; x++)
		{
			const uint32_t xsector = static_cast<uint32_t>(x) / 8;
			auto t = (xsector + ysector);

			texture_data[x * channel_count + y * channel_count * texture_width + 0] = (xsector + ysector) % 2 ? purple[0] : black[0];
			texture_data[x * channel_count + y * channel_count * texture_width + 1] = (xsector + ysector) % 2 ? purple[1] : black[1];
			texture_data[x * channel_count + y * channel_count * texture_width + 2] = (xsector + ysector) % 2 ? purple[2] : black[2];
			texture_data[x * channel_count + y * channel_count * texture_width + 3] = (xsector + ysector) % 2 ? purple[3] : black[3];
		}
	}

	return create_texture(device, 0, texture_width, texture_height, channel_count, false, texture_data);
	// default_texture_ = std::move(VulkanTexture(context_, 0, texture_width, texture_height, false, texture_data));
}

void Texture::flush_data(Buffer& staging_buffer, VkDeviceSize image_size, VkFormat image_format)
{
	staging_buffer.load_data(data_.data(), 0, image_size, 0);

	VkCommandPool pool = device_->get_graphics_command_pool();
	VkQueue queue = device_->get_graphics_queue();
	CommandBuffer temp_buffer = CommandBuffer::begin_single_time_commands(device_, pool);

	// Transition the layout to the optimal for receiving data
	image_.transition_layout(temp_buffer, image_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy data from the buffer
	image_.copy_from_buffer(temp_buffer, staging_buffer);

	// Transition to optimal read layout
	image_.transition_layout(temp_buffer, image_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	CommandBuffer::end_single_time_commands(device_, temp_buffer, queue);

	generation_++;
}

StatusOptional<Handle<VkSampler>, Status, Status::SUCCESS> Texture::create_sampler(Device *device)
{
	assert(device != nullptr);

	Handle<VkSampler> return_handle;

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
	auto result = vkCreateSampler(device->get_logical_device(), &sampler_info, nullptr, return_handle.ptr());
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

	return return_handle;
}

StatusOptional<VkFormat, Status, Status::SUCCESS> Texture::compute_format(uint8_t channel_count)
{
	switch (channel_count)
	{
		case 1:
			return VK_FORMAT_R8_UNORM;
		break;
		case 2:
			return VK_FORMAT_R8G8_UNORM;
		break;
		case 3:
			return VK_FORMAT_R8G8B8_UNORM;
		break;
		case 4:
			return VK_FORMAT_R8G8B8A8_UNORM;
		break;
		default:
			return Status::FLOWFORGE_UNSUPPORTED_CHANNEL_COUNT;
	}
}
}// namespace flwfrg::vk