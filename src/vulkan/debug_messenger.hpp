#pragma once
#include "util/handle.hpp"

namespace flwfrg::vk
{
class Instance;

class DebugMessenger
{
public:
	DebugMessenger() = default;
	explicit DebugMessenger(Instance* instance);
	~DebugMessenger();

	// Copy
	DebugMessenger(const DebugMessenger &) = delete;
	DebugMessenger &operator=(const DebugMessenger &) = delete;
	// Move
	DebugMessenger(DebugMessenger &&other) noexcept = default;
	DebugMessenger &operator=(DebugMessenger &&other) noexcept = default;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData);

	static VkResult createDebugUtilsMessengerEXT(
		VkInstance instance_,
		const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
		const VkAllocationCallbacks *pAllocator,
		VkDebugUtilsMessengerEXT *pDebugMessenger);

	static void destroyDebugUtilsMessengerEXT(
			VkInstance instance_,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks *pAllocator);

private:
	Instance* instance_ = nullptr;

	Handle<VkDebugUtilsMessengerEXT> handle_;
};

}