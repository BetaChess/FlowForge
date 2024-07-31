#pragma once


#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "util/handle.hpp"

namespace flwfrg::vk
{

class Instance
{
public:
	explicit Instance(bool enable_validation_layers = true);
	~Instance();

	// Copy
	Instance(const Instance &) = delete;
	Instance &operator=(const Instance &) = delete;
	// Move
	Instance(Instance &&other) noexcept = default;
	Instance &operator=(Instance &&other) noexcept = default;

	[[nodiscard]] VkInstance handle() const { return instance_; };

private:
	bool enable_validation_layers_;

	Handle<VkInstance> instance_;

	[[nodiscard]] static bool validation_layers_supported(const std::vector<const char *> &layers);
	[[nodiscard]] static std::vector<const char *> get_required_extensions(bool enable_validation_layers);
	static void check_glfw_required_instance_extensions(bool enable_validation_layers);

	std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
};

}


