#include "pch.hpp"

#include "vulkan_context.hpp"


namespace flwfrg::vk
{

std::expected<VulkanContext, Status> VulkanContext::create(const CreateInfo &create_info)
{
    // Create the instance
    Instance::CreateInfo instance_create_info
    {
        .application_name = create_info.application_name,
        .application_major_version = create_info.application_major_version,
        .application_minor_version = create_info.application_minor_version,
        .application_patch_version = create_info.application_patch_version,
        .vulkan_api_version = create_info.vulkan_api_version,
        .validation_layers = create_info.validation_layers,
        .extensions = create_info.extensions
    };
    auto instance = Instance::create(instance_create_info);

    if (!instance)
    {
        return std::unexpected(instance.error());
    }

    VulkanContext ret;
    ret.instance_ = std::move(instance.value());

    // Create the debug messenger (if needed)
    if (!create_info.validation_layers.empty())
    {
        auto debug_messenger = DebugMessenger::create(ret.instance_.handle());
        if (!debug_messenger)
        {
            return std::unexpected(debug_messenger.error());
        }
        ret.debug_messenger_ = std::move(debug_messenger.value());
    }

    return ret;
}

std::vector<const char *> VulkanContext::get_default_validation_layers()
{
    std::vector<const char *> ret{
        "VK_LAYER_KHRONOS_validation"
    };
    return ret;
}

std::vector<const char *> VulkanContext::get_default_extensions()
{
    std::vector<const char *> ret{
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    return ret;
}

} // namespace flwfrg::vk
