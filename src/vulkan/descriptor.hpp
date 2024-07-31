#pragma once
#include "util/handle.hpp"


#include <vulkan/vulkan_core.h>


namespace flwfrg::vk
{
class Device;

class DescriptorPool
{
public:
	DescriptorPool() = default;
	DescriptorPool(Device *device, VkDescriptorPoolCreateInfo create_info);// TODO: Make custom create_info struct.
	~DescriptorPool();

	// Copy
	DescriptorPool(const DescriptorPool &) = delete;
	DescriptorPool &operator=(const DescriptorPool &) = delete;
	// Move
	DescriptorPool(DescriptorPool &&other) noexcept = default;
	DescriptorPool &operator=(DescriptorPool &&other) noexcept = default;

	[[nodiscard]] constexpr VkDescriptorPool handle() const { return handle_; }

private:
	Device *device_ = nullptr;

	Handle<VkDescriptorPool> handle_{};
};

class DescriptorSetLayout
{
public:
	DescriptorSetLayout() = default;
	DescriptorSetLayout(Device *device, VkDescriptorSetLayoutCreateInfo create_info);// TODO: Make custom create_info struct.
	~DescriptorSetLayout();

	// Copy
	DescriptorSetLayout(const DescriptorSetLayout &) = delete;
	DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;
	// Move
	DescriptorSetLayout(DescriptorSetLayout &&other) noexcept = default;
	DescriptorSetLayout &operator=(DescriptorSetLayout &&other) noexcept = default;

	[[nodiscard]] constexpr VkDescriptorSetLayout handle() const { return handle_; }

private:
	Device *device_ = nullptr;

	Handle<VkDescriptorSetLayout> handle_;
};


}// namespace flwfrg::vk
