#pragma once

#include "debug_messenger.hpp"
#include "device.hpp"
#include "instance.hpp"
#include "util/status_optional.hpp"

#include <array>
#include <expected>
#include <string>

#ifndef MAXDEVICESINVKCONTEXT
#define MAXDEVICESINVKCONTEXT 4
#endif

namespace flwfrg::vk
{

class VulkanContext
{
public:
    struct CreateInfo
    {
        std::string application_name = "Unnamed Application";
        uint32_t application_major_version = 1;
        uint32_t application_minor_version = 0;
        uint32_t application_patch_version = 0;
        uint32_t vulkan_api_version = VK_API_VERSION_1_0;
        std::vector<const char*> validation_layers;
        std::vector<const char*> extensions;
    };

    VulkanContext() = default;

    static std::expected<VulkanContext, Status> create(const CreateInfo& create_info);
    static std::vector<const char*> get_default_validation_layers();
    static std::vector<const char*> get_default_extensions();

private:
    Instance instance_{};
    DebugMessenger debug_messenger_{};
    std::array<Device, MAXDEVICESINVKCONTEXT> devices_;
    uint16_t used_device_slots_ = 0;
};

}