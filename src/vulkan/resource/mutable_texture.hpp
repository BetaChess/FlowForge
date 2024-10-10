#pragma once

#include "texture.hpp"
#include "vulkan/buffer.hpp"

#include <ranges>

namespace flwfrg::vk
{

// TODO: Updating is waaaay too slow. Improve it.
class MutableTexture : public Texture
{
public:
	MutableTexture() = default;
	~MutableTexture() = default;

	// Copy
	MutableTexture(const MutableTexture &) = delete;
	MutableTexture &operator=(const MutableTexture &) = delete;
	// Move
	MutableTexture(MutableTexture &&other) noexcept = default;
	MutableTexture &operator=(MutableTexture &&other) noexcept = default;

	void update_texture(const std::ranges::forward_range auto& data_range, uint32_t offset)
	{
		// TODO: Might want this to be an actual check? Not just debug
		assert(data_range.size() + offset <= data_.size() && "Trying to update texture outside of data boundary");

		for (size_t i = 0; i < data_range.size(); i++)
			data_[i + offset] = data_range[i];

		flush_data(staging_buffer_, data_.size(), image_format_);
	};


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

private:
	Buffer staging_buffer_;

	VkDeviceSize image_size_{};
	VkFormat image_format_{};
};

}