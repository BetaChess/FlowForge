#pragma once

#include "vulkan/image.hpp"
#include "vulkan/util/constants.hpp"
#include "vulkan/util/status_optional.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace flwfrg::vk
{
class Device;

class Texture
{
public:
	Texture() = default;
	virtual ~Texture();

	// Copy
	Texture(const Texture &) = delete;
	Texture &operator=(const Texture &) = delete;
	// Move
	Texture(Texture &&other) noexcept = default;
	Texture &operator=(Texture &&other) noexcept = default;

	[[nodiscard]] inline uint32_t get_id() const { return id_; }
	[[nodiscard]] inline uint32_t get_width() const { return width_; }
	[[nodiscard]] inline uint32_t get_height() const { return height_; }
	[[nodiscard]] inline uint8_t get_channel_count() const { return channel_count_; }
	[[nodiscard]] inline bool has_transparency() const { return has_transparency_; }
	[[nodiscard]] inline uint32_t get_generation() const { return generation_; }

	[[nodiscard]] virtual const Image &get_image() const = 0;
	[[nodiscard]] VkSampler get_sampler() const { return sampler_; }

protected:
	Device *device_ = nullptr;

	uint32_t id_ = constant::invalid_id;
	uint32_t width_ = 0;
	uint32_t height_ = 0;
	uint8_t channel_count_ = 0;
	bool has_transparency_ = false;
	uint32_t generation_ = constant::invalid_generation;

	Handle<VkSampler> sampler_{};

	static StatusOptional<Handle<VkSampler>, Status, Status::SUCCESS> create_sampler(Device* device);
	static StatusOptional<VkFormat, Status, Status::SUCCESS> compute_format(uint8_t channel_count);
};

}// namespace flwfrg::vk