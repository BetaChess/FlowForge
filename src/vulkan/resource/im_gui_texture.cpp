#include "pch.hpp"

#include "im_gui_texture.hpp"

#include <imgui_impl_vulkan.h>

#include "vulkan/buffer.hpp"
#include "vulkan/command_buffer.hpp"
#include "vulkan/device.hpp"

namespace flwfrg::vk
{

ImGuiTexture::ImGuiTexture() = default;
ImGuiTexture::~ImGuiTexture() { free_descriptors(); }

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
    if (next_texture_index_ != current_texture_index_)
    {
        updateFences_[next_texture_index_].wait(std::numeric_limits<uint64_t>::max());
        current_texture_index_ = next_texture_index_;
    }

    next_texture_index_ = (current_texture_index_ + 1) % images_.size();

    updateFences_[next_texture_index_].wait(std::numeric_limits<uint64_t>::max());
    updateFences_[next_texture_index_].reset();

    flush_data(data, next_texture_index_);
}

Status ImGuiTexture::resize(uint32_t width, uint32_t height)
{
    if (width == width_ && height == height_)
        return Status::SUCCESS;

    std::vector<uint8_t> data(width * height * channel_count_, 255);
    return resize(width, height, channel_count_, data);
}

StatusOptional<ImGuiTexture, Status, Status::SUCCESS>
ImGuiTexture::create_imgui_texture(Device *device, uint32_t width, uint32_t height, uint8_t channel_count,
                                   bool has_transparency, std::vector<uint8_t> data)
{
    assert(device != nullptr);

    ImGuiTexture return_texture;
    return_texture.device_ = device;
    return_texture.has_transparency_ = has_transparency;
    return_texture.generation_ = constant::invalid_generation;

    // Create the command buffers
    for (size_t i = 0; i < return_texture.command_buffers_.size(); i++)
    {
        return_texture.command_buffers_[i] =
                CommandBuffer{return_texture.device_, return_texture.device_->get_graphics_command_pool(), true};
    }

    {
        auto ret_status = return_texture.resize(width, height, channel_count, data);
        if (ret_status != Status::SUCCESS)
            return ret_status;
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

void ImGuiTexture::free_descriptors()
{
    if (descriptor_sets_[0] != VK_NULL_HANDLE)
    {
        vkDeviceWaitIdle(device_->get_logical_device());
        for (VkDescriptorSet descriptor : descriptor_sets_)
            ImGui_ImplVulkan_RemoveTexture(descriptor);
    }
}

Status ImGuiTexture::resize(uint32_t width, uint32_t height, uint8_t channel_count, std::span<uint8_t> data)
{
    VkDeviceSize image_size = width * height * channel_count;
    assert(image_size == data.size());

    width_ = width;
    height_ = height;
    channel_count_ = channel_count;
    image_size_ = image_size;

    auto image_format = compute_format(channel_count_);
    if (image_format.status() != Status::SUCCESS)
        return image_format.status();

    image_format_ = image_format.value();

    free_descriptors();

    // Create the images
    for (size_t i = 0; i < images_.size(); i++)
    {
        images_[i].~Image();
        images_[i] = Image(device_, width_, height_, image_format_, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                   VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT, true);
    }

    // Create the fences
    for (size_t i = 0; i < updateFences_.size(); i++)
    {
        updateFences_[i].~Fence();
        updateFences_[i] = Fence{device_, true};
    }

    // Create staging buffer
    VkBufferUsageFlagBits usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags memory_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    for (size_t i = 0; i < images_.size(); i++)
    {
        staging_buffers_[i].~Buffer();
        staging_buffers_[i] = Buffer{device_, image_size, usage, memory_flags, true};
        updateFences_[i].reset();
        flush_data(data, i);
    }

    // Create sampler
    {
        auto sampler = create_sampler(device_);
        if (sampler.status() != Status::SUCCESS)
            return sampler.status();

        vkDestroySampler(device_->get_logical_device(), sampler_, nullptr);
        sampler_ = std::move(sampler.value());
    }

    // Create the descriptor sets
    for (size_t i = 0; i < descriptor_sets_.size(); i++)
    {
        descriptor_sets_[i] = make_handle(ImGui_ImplVulkan_AddTexture(sampler_, images_[i].get_image_view(),
                                                                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
    }

    return Status::SUCCESS;
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
