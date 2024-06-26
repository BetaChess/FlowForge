#pragma once

#include "renderer/vulkan/image.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace flwfrg
{

class VulkanContext;

class VulkanTexture
{
public:
	struct Data {
		glm::vec4 color{};
	};

public:
	VulkanTexture() = default;
	VulkanTexture(VulkanContext *context,
				  uint32_t id,
				  uint32_t width,
				  uint32_t height,
				  bool has_transparency,
				  std::vector<Data> data);

	~VulkanTexture();

	// Move constructor
	VulkanTexture(VulkanTexture &&other) noexcept;

	// Move assignment operator
	VulkanTexture &operator=(VulkanTexture &&other) noexcept;

	[[nodiscard]] inline uint32_t get_id() const { return id_; }
	[[nodiscard]] inline uint32_t get_width() const { return width_; }
	[[nodiscard]] inline uint32_t get_height() const { return height_; }
	// [[nodiscard]] inline uint8_t get_channel_count() const { return channel_count_; }
	[[nodiscard]] inline bool get_has_transparency() const { return has_transparency_; }
	[[nodiscard]] inline uint32_t get_generation() const { return generation_; }

	inline const VulkanImage &get_image() const { return image_; }
	[[nodiscard]] VkSampler get_sampler() const { return sampler_; }

	bool load_texture_from_file(std::string texture_name);

private:
	VulkanContext *context_ = nullptr;

	uint32_t id_ = std::numeric_limits<uint32_t>::max();
	uint32_t width_ = 0;
	uint32_t height_ = 0;
	// uint8_t channel_count_ = 0;
	bool has_transparency_ = false;
	uint32_t generation_ = std::numeric_limits<uint32_t>::max();
	std::vector<Data> data_{};

	VulkanImage image_{};
	VkSampler sampler_ = VK_NULL_HANDLE;
};

}// namespace flwfrg