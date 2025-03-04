#include "pch.hpp"

#include "im_gui_texture.hpp"

#include <imgui_impl_vulkan.h>

#include "vulkan/buffer.hpp"
#include "vulkan/command_buffer.hpp"
#include "vulkan/device.hpp"

namespace flwfrg::vk
{

ImGuiTexture::ImGuiTexture() = default;
ImGuiTexture::~ImGuiTexture()
{
    for (VkDescriptorSet descriptor : descriptor_sets_)
        ImGui_ImplVulkan_RemoveTexture(descriptor);
}

void ImGuiTexture::update_state()
{
    if (current_texture_index_ != next_texture_index_)
    {
        VkFence fence = updateFences_[next_texture_index_].get_handle();
        if (vkGetFenceStatus(device_->get_logical_device(), fence) == VK_SUCCESS)
        {
            current_texture_index_ = next_texture_index_;
        }
    }
}

const Image &ImGuiTexture::get_image() const { return images_[current_texture_index_]; }

VkDescriptorSet ImGuiTexture::get_descriptor_set() const { return descriptor_sets_[current_texture_index_]; }

void ImGuiTexture::update_texture(std::span<uint8_t> data)
{
    next_texture_index_ = (current_texture_index_ + 1) % images_.size();

    updateFences_[next_texture_index_].wait(std::numeric_limits<uint64_t>::max());
    updateFences_[next_texture_index_].reset();

    flush_data(data, next_texture_index_);
}

StatusOptional<ImGuiTexture, Status, Status::SUCCESS>
ImGuiTexture::create_imgui_texture(Device *device, uint32_t width, uint32_t height, uint8_t channel_count,
                                   bool has_transparency, std::vector<uint8_t> data)
{
    assert(device != nullptr);

    ImGuiTexture return_texture;
    return_texture.device_ = device;
    return_texture.width_ = width;
    return_texture.height_ = height;
    return_texture.channel_count_ = channel_count;
    return_texture.has_transparency_ = has_transparency;
    return_texture.generation_ = constant::invalid_generation;

    VkDeviceSize image_size = return_texture.width_ * return_texture.height_ * return_texture.channel_count_;
    assert(image_size == data.size());

    return_texture.image_size_ = image_size;

    auto image_format = compute_format(return_texture.channel_count_);
    if (image_format.status() != Status::SUCCESS)
        return image_format.status();

    return_texture.image_format_ = image_format.value();

    // Create the images
    for (size_t i = 0; i < return_texture.images_.size(); i++)
    {
        return_texture.images_[i] = Image(return_texture.device_, return_texture.width_, return_texture.height_,
                                          return_texture.image_format_, VK_IMAGE_TILING_OPTIMAL,
                                          VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                  VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, true);
    }

    // Create the command buffers
    for (size_t i = 0; i < return_texture.command_buffers_.size(); i++)
    {
        return_texture.command_buffers_[i] = CommandBuffer{return_texture.device_, return_texture.device_->get_graphics_command_pool(), true};
    }

    // Create the fences
    for (size_t i = 0; i < return_texture.updateFences_.size(); i++)
    {
        return_texture.updateFences_[i] = Fence{return_texture.device_, true};
    }

    // Create staging buffer
    VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for (size_t i = 0; i < return_texture.images_.size(); i++)
    {
        return_texture.staging_buffers_[i] = Buffer{return_texture.device_, image_size, usage, memory_flags, true};
        return_texture.updateFences_[i].reset();
        return_texture.flush_data(data, i);
    }

    // Create sampler
    {
        auto sampler = create_sampler(return_texture.device_);
        if (sampler.status() != Status::SUCCESS)
            return sampler.status();

        return_texture.sampler_ = std::move(sampler.value());
    }

    // Create the descriptor sets
    for (size_t i = 0; i < return_texture.descriptor_sets_.size(); i++)
    {
        return_texture.descriptor_sets_[i] =
                make_handle(ImGui_ImplVulkan_AddTexture(return_texture.sampler_, return_texture.images_[i].get_image_view(),
                                            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }


    return_texture.generation_ = 0;

    return std::move(return_texture);
}

StatusOptional<ImGuiTexture, Status, Status::SUCCESS>
ImGuiTexture::create_imgui_texture(Device *device, uint32_t width, uint32_t height, uint8_t channel_count)
{
    std::vector<uint8_t> data(width * height * channel_count, 255);
    return std::move(create_imgui_texture(device, width, height, channel_count, false, data));
}
void ImGuiTexture::flush_data(std::span<uint8_t> data, size_t image_index)
{
    Buffer &staging_buffer = staging_buffers_[image_index];

    staging_buffer.load_data(data.data(), 0, image_size_, 0);

    VkCommandPool pool = device_->get_graphics_command_pool();
    VkQueue queue = device_->get_graphics_queue();

    command_buffers_[image_index].reset();
    command_buffers_[image_index].begin(false, true, false);

    // Transition the layout to the optimal for receiving data
    images_[image_index].transition_layout(command_buffers_[image_index], image_format_, VK_IMAGE_LAYOUT_UNDEFINED,
                                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Copy data from the buffer
    images_[image_index].copy_from_buffer(command_buffers_[image_index], staging_buffer);

    // Transition to optimal read layout
    images_[image_index].transition_layout(command_buffers_[image_index], image_format_,
                                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    command_buffers_[image_index].end();
    command_buffers_[image_index].submit(queue, VK_NULL_HANDLE, VK_NULL_HANDLE,
                                         updateFences_[image_index].get_handle());

    generation_++;
}

} // namespace flwfrg::vk
