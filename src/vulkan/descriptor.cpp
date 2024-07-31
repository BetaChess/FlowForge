#include "pch.hpp"

#include "descriptor.hpp"

#include "device.hpp"

namespace flwfrg::vk
{

DescriptorPool::DescriptorPool(Device *device, VkDescriptorPoolCreateInfo create_info)
	: device_{device}
{
	assert(device_ != nullptr);

	if (vkCreateDescriptorPool(device_->get_logical_device(), &create_info, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

DescriptorPool::~DescriptorPool()
{
	if (handle_.not_null())
	{
		vkDestroyDescriptorPool(device_->get_logical_device(), handle_, nullptr);
		handle_ = make_handle<VkDescriptorPool>(VK_NULL_HANDLE);
	}
}

DescriptorSetLayout::DescriptorSetLayout(Device *device, VkDescriptorSetLayoutCreateInfo create_info)
	: device_{device}
{
	assert(device_ != nullptr);

	if (vkCreateDescriptorSetLayout(device_->get_logical_device(), &create_info, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

DescriptorSetLayout::~DescriptorSetLayout()
{
	if (handle_.not_null())
	{
		vkDestroyDescriptorSetLayout(device_->get_logical_device(), handle_, nullptr);
		handle_ = make_handle<VkDescriptorSetLayout>(VK_NULL_HANDLE);
	}
}

}// namespace flwfrg::vk