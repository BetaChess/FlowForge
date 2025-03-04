#include "pch.hpp"

#include "mutable_texture.hpp"

#include "vulkan/device.hpp"

namespace flwfrg::vk
{


const Image &MutableTexture::get_image() const {return images_[0]; }
StatusOptional<MutableTexture, Status, Status::SUCCESS> MutableTexture::create_mutable_texture(Device *device, uint32_t id, uint32_t width, uint32_t height, uint8_t channel_count, bool has_transparency, std::vector<uint8_t> data)
{
	assert(device != nullptr);

	MutableTexture return_texture{device};
	return_texture.device_ = device;
	return_texture.id_ = id;
	return_texture.width_ = width;
	return_texture.height_ = height;
	return_texture.channel_count_ = channel_count;
	return_texture.has_transparency_ = has_transparency;
	return_texture.generation_ = constant::invalid_generation;

	VkDeviceSize image_size = return_texture.width_ * return_texture.height_ * return_texture.channel_count_;
	assert(image_size == data.size());

	return_texture.image_size_ = image_size;

	auto image_format = compute_format(return_texture.channel_count_);
	if (image_format.status() != Status::SUCCESS)
		return image_format.status();

	return_texture.image_format_ = image_format.value();

	for (size_t i = 0; i < 3; i++)
	{
		return_texture.images_[i] = Image(return_texture.device_,
										  return_texture.width_, return_texture.height_,
										  return_texture.image_format_,
										  VK_IMAGE_TILING_OPTIMAL,
										  VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
										  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
										  VK_IMAGE_ASPECT_COLOR_BIT,
										  true);
	}


	// Create staging buffer
	VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	for (size_t i = 0; i < 3; i++)
	{
		return_texture.staging_buffers_[i] = Buffer{return_texture.device_, image_size, usage, memory_flags, true};
		return_texture.updateFences_[i].reset();
		return_texture.flush_data(i, return_texture.staging_buffers_[i], data, image_size, return_texture.image_format_);
	}

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

StatusOptional<MutableTexture, Status, Status::SUCCESS> MutableTexture::create_mutable_texture(Device *device, uint32_t id, uint32_t width, uint32_t height, uint8_t channel_count)
{
	std::vector<uint8_t> data;
	data.resize(width * height * channel_count);
	if (channel_count == 4)
	{
		for (size_t i = 3; i < data.size(); i += 4)
			data[i] = 255;
	}

	return create_mutable_texture(device, id, width, height, channel_count, false, data);
}

MutableTexture::MutableTexture(Device *device)
	: command_buffers_{CommandBuffer{device, device->get_graphics_command_pool(), true}, CommandBuffer{device, device->get_graphics_command_pool(), true}, CommandBuffer{device, device->get_graphics_command_pool(), true}},
updateFences_{{Fence{device, true}, Fence{device, true}, Fence{device, true}}}
{
}

void MutableTexture::flush_data(size_t image_index, Buffer &staging_buffer, const std::span<uint8_t> data, VkDeviceSize image_size, VkFormat image_format)
{
	staging_buffer.load_data(data.data(), 0, image_size, 0);

	VkCommandPool pool = device_->get_graphics_command_pool();
	VkQueue queue = device_->get_graphics_queue();

	command_buffers_[image_index].reset();
	command_buffers_[image_index].begin(false, true, false);

	// Transition the layout to the optimal for receiving data
	images_[image_index].transition_layout(command_buffers_[image_index], image_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	// Copy data from the buffer
	images_[image_index].copy_from_buffer(command_buffers_[image_index], staging_buffer);

	// Transition to optimal read layout
	images_[image_index].transition_layout(command_buffers_[image_index], image_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	command_buffers_[image_index].end();
	command_buffers_[image_index].submit(queue, VK_NULL_HANDLE, VK_NULL_HANDLE, updateFences_[image_index].get_handle());

	generation_++;
}

}// namespace flwfrg::vk
