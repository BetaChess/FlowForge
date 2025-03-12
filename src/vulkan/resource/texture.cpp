#include "pch.hpp"

#include "texture.hpp"

#include "vulkan/buffer.hpp"
#include "vulkan/command_buffer.hpp"
#include "vulkan/device.hpp"

#include <stb_image.h>

namespace flwfrg::vk
{

Texture::~Texture()
{
    if (sampler_.not_null())
    {
        vkDeviceWaitIdle(device_->get_logical_device());
        vkDestroySampler(device_->get_logical_device(), sampler_, nullptr);
        FLOWFORGE_TRACE("Texture destroyed");
    }
}

StatusOptional<Handle<VkSampler>, Status, Status::SUCCESS> Texture::create_sampler(Device *device)
{
    assert(device != nullptr);

    Handle<VkSampler> return_handle;

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_TRUE;
    sampler_info.maxAnisotropy = 16;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    // Create the sampler and check the result
    auto result = vkCreateSampler(device->get_logical_device(), &sampler_info, nullptr, return_handle.ptr());
    if (result != VK_SUCCESS)
    {
        switch (result)
        {
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return {Status::OUT_OF_HOST_MEMORY};
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return {Status::OUT_OF_DEVICE_MEMORY};
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR:
            return {Status::INVALID_OPAQUE_CAPTURE_ADDRESS_KHR};
        default:
            return {Status::UNKNOWN_ERROR};
        }
    }

    return return_handle;
}

StatusOptional<VkFormat, Status, Status::SUCCESS> Texture::compute_format(uint8_t channel_count)
{
    switch (channel_count)
    {
    case 1:
        return VK_FORMAT_R8_UNORM;
    case 2:
        return VK_FORMAT_R8G8_UNORM;
    case 3:
        return VK_FORMAT_R8G8B8_UNORM;
    case 4:
        return VK_FORMAT_R8G8B8A8_UNORM;
    default:
        return Status::FLOWFORGE_UNSUPPORTED_CHANNEL_COUNT;
    }
}
} // namespace flwfrg::vk
