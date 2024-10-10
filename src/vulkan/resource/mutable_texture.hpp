#pragma once

#include "texture.hpp"
#include "vulkan/buffer.hpp"

namespace flwfrg::vk
{

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
};

}