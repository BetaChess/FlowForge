#include "pch.hpp"

#include "mutable_texture.hpp"

#include "vulkan/device.hpp"

namespace flwfrg::vk
{

MutableTexture::~MutableTexture()
{
	vkDeviceWaitIdle(device_->get_logical_device());

	if (sampler_.not_null())
	{
		vkDestroySampler(device_->get_logical_device(), sampler_, nullptr);
	}
}
StatusOptional<MutableTexture, Status, Status::SUCCESS> MutableTexture::create_mutable_texture(Device *device, uint32_t id, uint32_t width, uint32_t height, uint8_t channel_count, bool has_transparency, std::vector<uint8_t> data)
{
	assert(device != nullptr);

	MutableTexture return_texture;
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
	return_texture.staging_buffer_ = Buffer{return_texture.device_, image_size, usage, memory_flags, true};
	return_texture.flush_data(return_texture.staging_buffer_, image_size, image_format.value());

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

}// namespace flwfrg::vk
