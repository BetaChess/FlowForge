#pragma once

#include "texture.hpp"

namespace flwfrg::vk
{

class StaticTexture : public Texture
{
public:
	[[nodiscard]] const Image &get_image() const override;

	Status load_texture_from_file(std::string texture_name);

	static StatusOptional<StaticTexture, Status, Status::SUCCESS> create_texture(
			Device *device,
			uint32_t id,
			uint32_t width,
			uint32_t height,
			uint8_t channel_count,
			bool has_transparency,
			std::vector<uint8_t> data);

	static StatusOptional<StaticTexture, Status, Status::SUCCESS> generate_default_texture(Device *device);

protected:
	Image image_{};
    std::vector<uint8_t> data_;

	void flush_data(Buffer &staging_buffer, VkDeviceSize image_size, VkFormat image_format);
};

}