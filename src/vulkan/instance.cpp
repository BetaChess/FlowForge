#include "pch.hpp"

#include "instance.hpp"

#include "debug_messenger.hpp"

#include <stdexcept>
#include <unordered_set>

namespace flwfrg::vk
{
Instance::Instance(bool enable_validation_layers)
	: enable_validation_layers_{enable_validation_layers}
{
	FLOWFORGE_INFO("Creating Vulkan instance");

	// Check if the validation layers are enabled and supported.
	if (enable_validation_layers_ && !validation_layers_supported(validationLayers))
	{
		// If they are enabled but not supported, throw a runtime error.
		throw std::runtime_error("Validation layers requested, but not available! ");
	}

	// Create the appinfo struct.
	VkApplicationInfo appInfo{};
	// Specify the struct as a type application info. (a struct used to give information about the instance).
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	// Set the name
	appInfo.pApplicationName = "FlowForge";
	// Specify the application version
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	// A custom engine is used.
	appInfo.pEngineName = "FlowForge";
	// Specify the engine version
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	// Specify the vulkan API version.
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Create the instance_ create info
	VkInstanceCreateInfo createInfo{};
	// Specify that this struct is the info to create the instance.
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// give it the application info created earlier.
	createInfo.pApplicationInfo = &appInfo;

	// Get and enable the glfw extensions
	auto extensions = get_required_extensions(enable_validation_layers_);
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());// The number of enabled extensions
	createInfo.ppEnabledExtensionNames = extensions.data();                     // The names of the actual extensions

	// create a temporary debugger for creation of the Vulkan instance_
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	// check if validation layers are enabled.
	if (enable_validation_layers_)
	{
		// Specify the enabled validation layers
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());// The number of validation layers
		createInfo.ppEnabledLayerNames = validationLayers.data();                     // The names of the validation layers

		// Use a helper function to populate the createInfo for the debugMessenger.
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
										  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
										  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
									  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
									  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = DebugMessenger::debugCallback;
		debugCreateInfo.pUserData = nullptr;// Optional

		// Give the struct the creation info.
		createInfo.pNext = &debugCreateInfo;
	} else
	{
		// Specify that no layers are enabled (0)
		createInfo.enabledLayerCount = 0;

		// Give it a nullptr to the creation info.
		createInfo.pNext = nullptr;
	}

	// Finally, create the instance_
	if (vkCreateInstance(&createInfo, nullptr, instance_.ptr()) != VK_SUCCESS)
	{
		// If the instance failed to be created, throw a runtime error.
		throw std::runtime_error("failed to create instance_!");
	}

	check_glfw_required_instance_extensions(enable_validation_layers_);
}

Instance::~Instance()
{
	if (instance_.not_null())
	{
		vkDestroyInstance(instance_, nullptr);
		FLOWFORGE_INFO("Vulkan instance destroyed");
	}
}

bool Instance::validation_layers_supported(const std::vector<const char *> &layers)
{
	// Get the validation layer count
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	// Create a vector to store the available layers
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	// print the available layers
	{
		std::stringstream ss;

		ss << "\n available layers:";

		for (const auto &layer: availableLayers)
			ss << "\n\t" << layer.layerName;

		FLOWFORGE_TRACE(ss.str());
	}

	// Iterate through each layer name in the list of required layers
	for (const char *layerName: layers)
	{
		bool layerFound = false;

		// Check if the layer is supported by iterating through the supported layers
		for (const auto &layerProperties: availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		// If any layer is not found. Return false.
		if (!layerFound)
		{
			FLOWFORGE_ERROR("Layer was not found during Validation Layer Support checking! Layer name is: {}", layerName);
			return false;
		}
	}

	return true;
}

std::vector<const char *> Instance::get_required_extensions(bool enable_validation_layers)
{
	// Get the number of extensions required by glfw.
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Put them in the vector of required extensions
	std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// If validation layers are enabled. Throw in the Debug Utils extension.
	if (enable_validation_layers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	// Return the vector.
	return extensions;
}

void Instance::check_glfw_required_instance_extensions(bool enable_validation_layers)
{
	// Create a variable to store the number of supported extensions
	uint32_t extensionCount = 0;

	// Get the number of avaliable extensions. (The middle parameter of the function writes this to the specified memory location)
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	// Change the size of the extensions vetor to match the number of avaliable extensions.
	std::vector<VkExtensionProperties> extensions(extensionCount);
	// Write the extensions to the vector.
	// (the last parameter is a memory location to the place the extension properties should be written,
	//		in this case, the data location of the vector)
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// List out the avaliable extensions and put the names in a string data structure.
	std::unordered_set<std::string> available;
	{
		std::stringstream ss;
		ss << "\n available extensions:";
		for (const auto &extension: extensions)
		{
			ss << "\n\t" << extension.extensionName;
			available.insert(extension.extensionName);
		}

		FLOWFORGE_TRACE(ss.str());
	}

	{
		std::stringstream ss;
		// List out the required extensions.
		ss << "\n required extensions:";
		// Get the required extensions.
		auto requiredExtensions = get_required_extensions(enable_validation_layers);
		// Iterate through the required extensions.
		for (const auto &required: requiredExtensions)
		{
			ss << "\n\t" << required;
			// If the required extension is not available, throw a runtime error.
			if (available.find(required) == available.end())
			{
				FLOWFORGE_FATAL("Missing required glfw extension ({})", required);
				throw std::runtime_error("Missing required glfw extension");
			}
		}

		FLOWFORGE_TRACE(ss.str());
	}
}

}// namespace flwfrg::vk