#pragma once

#include "texture.hpp"
#include "vulkan/buffer.hpp"
#include "vulkan/command_buffer.hpp"
#include "vulkan/fence.hpp"

#include <ranges>

namespace flwfrg::vk
{

// TODO: Updating is waaaay too slow. Improve it.
class MutableTexture : public Texture
{
public:
	~MutableTexture() override = default;

	// Copy
	MutableTexture(const MutableTexture &) = delete;
	MutableTexture &operator=(const MutableTexture &) = delete;
	// Move
	MutableTexture(MutableTexture &&other) noexcept = default;
	MutableTexture &operator=(MutableTexture &&other) noexcept = default;

	[[nodiscard]] const Image &get_image(uint64_t frame) const override;

	static StatusOptional<MutableTexture, Status, Status::SUCCESS> create_mutable_texture(
			Device *device,
			uint32_t id,
			uint32_t width,
			uint32_t height,
			uint8_t channel_count,
			bool has_transparency,
			std::vector<uint8_t> data);

	static StatusOptional<MutableTexture, Status, Status::SUCCESS> create_mutable_texture(
			Device *device,
			uint32_t id,
			uint32_t width,
			uint32_t height,
			uint8_t channel_count);

	void update_texture(uint64_t frame, const std::span<uint8_t> data)
	{
		size_t image_index = frame % 3;

		updateFences_[image_index].wait(std::numeric_limits<uint64_t>::max());
		updateFences_[image_index].reset();

		flush_data(image_index, staging_buffers_[image_index], data, data.size(), image_format_);
	}

protected:
	std::array<CommandBuffer, 3> command_buffers_;

	std::array<Fence, 3> updateFences_;
	std::array<Buffer, 3> staging_buffers_;
	std::array<Image, 3> images_;

	VkDeviceSize image_size_{};
	VkFormat image_format_{};

	explicit MutableTexture(Device* device);

	void flush_data(size_t image_index, Buffer &staging_buffer, const std::span<uint8_t> data, VkDeviceSize image_size, VkFormat image_format);
};

}// namespace flwfrg::vk