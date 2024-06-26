#pragma once

#include "device.hpp"
#include <vulkan/vulkan_core.h>


namespace flwfrg
{

class VulkanContext;
class VulkanCommandBuffer;
class VulkanBuffer;

class VulkanImage
{
public:
	VulkanImage() = default;
	VulkanImage(
			VulkanContext *context,
			uint32_t width,
			uint32_t height,
			VkFormat format,
			VkImageTiling tiling,
			VkImageUsageFlags usage,
			VkMemoryPropertyFlags memory_flags,
			VkImageAspectFlags aspect_flags,
			bool create_view = true);
	~VulkanImage();

	// Not copyable but movable
	VulkanImage(const VulkanImage &) = delete;
	VulkanImage &operator=(const VulkanImage &) = delete;
	VulkanImage(VulkanImage &&other) noexcept;
	VulkanImage &operator=(VulkanImage &&other) noexcept;

	void transition_layout(VulkanCommandBuffer &command_buffer,
						  VkFormat format,
						  VkImageLayout old_layout,
						  VkImageLayout new_layout);

	void copy_from_buffer(VulkanCommandBuffer& command_buffer, VulkanBuffer& buffer);

	[[nodiscard]] inline VkImage get_image_handle() const { return image_handle_; }
	[[nodiscard]] inline VkImageView get_image_view() const { return view_; }
	[[nodiscard]] inline uint32_t get_width() const { return width_; }
	[[nodiscard]] inline uint32_t get_height() const { return height_; }

private:
	VulkanContext *context_ = nullptr;

	VkImage image_handle_ = VK_NULL_HANDLE;
	VkDeviceMemory memory_ = VK_NULL_HANDLE;
	VkImageView view_ = VK_NULL_HANDLE;
	uint32_t width_ = 0;
	uint32_t height_ = 0;

	void view_create(VkFormat format, VkImageAspectFlags aspect_flags);

	friend VulkanContext;
};

}// namespace flwfrg