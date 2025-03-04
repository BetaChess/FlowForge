#pragma once

#include "texture.hpp"
#include "vulkan/command_buffer.hpp"
#include "vulkan/buffer.hpp"
#include "vulkan/fence.hpp"

namespace flwfrg::vk
{
class ImGuiTexture : public Texture
{
public:
    ImGuiTexture();
    ~ImGuiTexture() override;

    // Copy
    ImGuiTexture(const ImGuiTexture &) = delete;
    ImGuiTexture &operator=(const ImGuiTexture &) = delete;
    // Move
    ImGuiTexture(ImGuiTexture &&) noexcept = default;
    ImGuiTexture &operator=(ImGuiTexture &&) noexcept = default;

    void update_state();
    [[nodiscard]] const Image &get_image() const override;
    [[nodiscard]] VkDescriptorSet get_descriptor_set() const;

    void update_texture(std::span<uint8_t> data);

    Status resize(uint32_t width, uint32_t height);

    static StatusOptional<ImGuiTexture, Status, Status::SUCCESS>
    create_imgui_texture(Device *device, uint32_t width, uint32_t height, uint8_t channel_count, bool has_transparency,
                         std::vector<uint8_t> data);

    static StatusOptional<ImGuiTexture, Status, Status::SUCCESS>
    create_imgui_texture(Device *device, uint32_t width, uint32_t height, uint8_t channel_count);

private:
    std::array<CommandBuffer, 2> command_buffers_;

    std::array<Fence, 2> updateFences_;
    std::array<Buffer, 2> staging_buffers_;
    std::array<Image, 2> images_;
    std::array<Handle<VkDescriptorSet>, 2> descriptor_sets_{};

    uint8_t current_texture_index_ = 0;
    uint8_t next_texture_index_ = 0;

    VkDeviceSize image_size_{};
    VkFormat image_format_{};

    void free_descriptors();

    Status resize(uint32_t width, uint32_t height, uint8_t channel_count, std::span<uint8_t> data);

    void flush_data(std::span<uint8_t> data, size_t image_index);
};

} // namespace flwfrg::vk
