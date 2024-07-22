#include "pch.hpp"

#include "buffer.hpp"

#include "command_buffer.hpp"
#include "device.hpp"

namespace flwfrg::vk
{


Buffer::Buffer(Device* device, uint64_t size, VkBufferUsageFlagBits usage, uint32_t memory_property_flags, bool bind_on_create)
	: device_{device}
{
	assert(device != nullptr);

	VkBufferCreateInfo buffer_create_info{};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = usage;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Only used in a single queue

	if (vkCreateBuffer(device->get_logical_device(), &buffer_create_info, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create buffer");
	}

	// Gather memory requirements
	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device->get_logical_device(), handle_, &memory_requirements);
	memory_index_ = device->find_memory_index(memory_requirements.memoryTypeBits, memory_property_flags_);
	if (memory_index_ == -1)
	{
		throw std::runtime_error("Failed to find suitable memory type");
	}

	// Allocate memory info
	VkMemoryAllocateInfo memory_allocate_info{};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = static_cast<uint32_t>(memory_index_);

	// Allocate the memory
	if (vkAllocateMemory(device->get_logical_device(), &memory_allocate_info, nullptr, memory_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate memory");
	}

	if (bind_on_create)
	{
		bind(0);
	}
}

Buffer::~Buffer()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device_->get_logical_device(), handle_, nullptr);
	}
	if (memory_ != VK_NULL_HANDLE)
	{
		vkFreeMemory(device_->get_logical_device(), memory_, nullptr);
	}
}

void Buffer::resize(uint64_t new_size, VkQueue queue, VkCommandPool pool)
{
	// Create new buffer
	Buffer new_buffer{device_, new_size, usage_, memory_property_flags_, true};

	// Copy the data
	copy_to(new_buffer, 0, new_size, 0, pool, VK_NULL_HANDLE, queue);

	// Wait for device to be idle
	vkDeviceWaitIdle(device_->get_logical_device());

	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device_->get_logical_device(), handle_, nullptr);
	}
	if (memory_ != VK_NULL_HANDLE)
	{
		vkFreeMemory(device_->get_logical_device(), memory_, nullptr);
	}

	// Move the new buffer to this
	*this = std::move(new_buffer);
}

void *Buffer::lock_memory(uint64_t offset, uint64_t size, uint32_t flags)
{
	void *data;
	if (vkMapMemory(device_->get_logical_device(), memory_, offset, size, flags, &data) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to map memory");
	}
	locked_ = true;
	return data;
}

void Buffer::unlock_memory()
{
	vkUnmapMemory(device_->get_logical_device(), memory_);
	locked_ = false;
}

void Buffer::bind(uint64_t offset)
{
	vkBindBufferMemory(device_->get_logical_device(), handle_, memory_, offset);
}

void Buffer::load_data(const void *data, uint64_t offset, uint64_t size, uint32_t flags)
{
	void *mapped_memory = lock_memory(offset, size, flags);
	memcpy(mapped_memory, data, size);
	unlock_memory();
}

void Buffer::copy_to(Buffer &dst, uint64_t dst_offset, uint64_t dst_size, uint64_t src_offset, VkCommandPool pool, VkFence fence, VkQueue queue)
{
	vkQueueWaitIdle(queue);

	// Create a one time use command buffer
	CommandBuffer command_buffer = CommandBuffer::begin_single_time_commands(device_, pool);

	// Copy the buffer
	VkBufferCopy copy_region{};
	copy_region.srcOffset = src_offset;
	copy_region.dstOffset = dst_offset;
	copy_region.size = dst_size;

	vkCmdCopyBuffer(command_buffer.get_handle(), handle_, dst.handle_, 1, &copy_region);

	// End the command buffer
	CommandBuffer::end_single_time_commands(device_, command_buffer, queue);
}

}// namespace flwfrg::vk
