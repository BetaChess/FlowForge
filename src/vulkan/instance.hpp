#pragma once

#include "util/handle.hpp"
#include "util/status_optional.hpp"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <expected>
#include <string_view>


namespace flwfrg::vk
{

class Instance
{
public:
    struct CreateInfo
    {
        std::string_view application_name;
        uint32_t application_major_version;
        uint32_t application_minor_version;
        uint32_t application_patch_version;
        uint32_t vulkan_api_version;
        const std::vector<const char*>& validation_layers;
        const std::vector<const char*>& extensions;
    };


	// explicit Instance(bool enable_validation_layers = true);
    Instance() = default;
	~Instance();

	// Copy
	Instance(const Instance &) = delete;
	Instance &operator=(const Instance &) = delete;
	// Move
	Instance(Instance &&other) noexcept = default;
	Instance &operator=(Instance &&other) noexcept = default;

	[[nodiscard]] VkInstance handle() const { return instance_; };


    static std::expected<Instance, Status> create(CreateInfo create_info);


private:
	// bool enable_validation_layers_;

	Handle<VkInstance> instance_;

	[[nodiscard]] static bool validation_layers_supported(const std::vector<const char *> &layers);
    [[nodiscard]] static bool extensions_supported(const std::vector<const char*> &extension_names);
	// [[nodiscard]] static std::vector<const char *> get_required_extensions(bool enable_validation_layers);
	// static void check_glfw_required_instance_extensions(bool enable_validation_layers);

	// std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
};

}


