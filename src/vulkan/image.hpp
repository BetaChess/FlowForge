#pragma once

#include "util/handle.hpp"


#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{

class CommandBuffer;
class Buffer;
class Device;

class Image
{
public:
	Image() = default;
	Image(Device *device,
		  uint32_t width,
		  uint32_t height,
		  VkFormat format,
		  VkImageTiling tiling,
		  VkImageUsageFlags usage,
		  VkMemoryPropertyFlags memory_flags,
		  VkImageAspectFlags aspect_flags,
		  bool create_view = true);
	~Image();

	// Copy
	Image(const Image &) = delete;
	Image &operator=(const Image &) = delete;
	// Move
	constexpr Image(Image &&other) noexcept = default;
	constexpr Image &operator=(Image &&other) noexcept = default;


	void transition_layout(CommandBuffer &command_buffer,
						   VkFormat format,
						   VkImageLayout old_layout,
						   VkImageLayout new_layout);

	void copy_from_buffer(CommandBuffer &command_buffer, Buffer &buffer);

	[[nodiscard]] inline VkImage get_image_handle() const { return image_handle_; }
	[[nodiscard]] inline VkImageView get_image_view() const { return view_; }
	[[nodiscard]] inline uint32_t get_width() const { return width_; }
	[[nodiscard]] inline uint32_t get_height() const { return height_; }

private:
	Device *device_ = nullptr;

	Handle<VkImage> image_handle_{};
	Handle<VkDeviceMemory> memory_{};
	Handle<VkImageView> view_{};
	uint32_t width_ = 0;
	uint32_t height_ = 0;

	void view_create(VkFormat format, VkImageAspectFlags aspect_flags);
};

}// namespace flwfrg::vk