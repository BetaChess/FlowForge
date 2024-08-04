#include "pch.hpp"

#include "debug_messenger.hpp"

#include "instance.hpp"

namespace flwfrg::vk
{


DebugMessenger::DebugMessenger(Instance *instance)
	: instance_{instance}
{
	FLOWFORGE_INFO("Setting Vulkan debug messenger");

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
	createInfo.pUserData = nullptr;// Optional

	// Try to create the debug messenger. Throw a runtime error if it failed.
	if (createDebugUtilsMessengerEXT(instance_->handle(), &createInfo, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

DebugMessenger::~DebugMessenger()
{
	if (handle_.not_null())
	{
		destroyDebugUtilsMessengerEXT(instance_->handle(), handle_, nullptr);
		FLOWFORGE_INFO("Vulkan debug callback destroyed");
	}
}

VkBool32 DebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
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

VkResult DebugMessenger::createDebugUtilsMessengerEXT(VkInstance instance_, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	{

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
				instance_,
				"vkCreateDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			return func(instance_, pCreateInfo, pAllocator, pDebugMessenger);
		} else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
}

void DebugMessenger::destroyDebugUtilsMessengerEXT(VkInstance instance_, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
				instance_,
				"vkDestroyDebugUtilsMessengerEXT");

		if (func != nullptr)
		{
			func(instance_, debugMessenger, pAllocator);
		}
	}
}
}// namespace flwfrg::vk