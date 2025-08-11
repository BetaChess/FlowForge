#include "pch.hpp"

#include "debug_messenger.hpp"

#include "instance.hpp"

namespace flwfrg::vk
{

DebugMessenger::~DebugMessenger()
{
    if (handle_.not_null())
    {
        assert(instance_.not_null());
        destroyDebugUtilsMessengerEXT(instance_, handle_, nullptr);
        FLOWFORGE_INFO("Vulkan debug callback destroyed");
    }
}

std::expected<DebugMessenger, Status> DebugMessenger::create(VkInstance instance)
{
    FLOWFORGE_INFO("Setting up Vulkan debug messenger");

    // Create the debug messenger create info.
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional

    DebugMessenger ret;
    ret.instance_ = make_handle(instance);

    // Try to create the debug messenger. Throw a runtime error if it failed.
    auto result = createDebugUtilsMessengerEXT(ret.instance_, &createInfo, nullptr, ret.handle_.ptr());
    if (result != VK_SUCCESS)
    {
        return std::unexpected(vk_result_to_flwfrg_status(result));
    }

    return ret;
}

VkBool32 DebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
{
    {

        // TODO: Seperate from normal flowforge errors
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            FLOWFORGE_ERROR("VK_VALIDATION {}", pCallbackData->pMessage);
        } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            FLOWFORGE_WARN("VK_VALIDATION {}", pCallbackData->pMessage);
        } else
        {
            FLOWFORGE_TRACE("VK_VALIDATION {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }
}

VkResult DebugMessenger::createDebugUtilsMessengerEXT(VkInstance instance_,
                                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                      const VkAllocationCallbacks *pAllocator,
                                                      VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    {

        auto func =
                (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            return func(instance_, pCreateInfo, pAllocator, pDebugMessenger);
        } else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
}

void DebugMessenger::destroyDebugUtilsMessengerEXT(VkInstance instance_, VkDebugUtilsMessengerEXT debugMessenger,
                                                   const VkAllocationCallbacks *pAllocator)
{
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_,
                                                                               "vkDestroyDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            func(instance_, debugMessenger, pAllocator);
        }
    }
}
} // namespace flwfrg::vk
