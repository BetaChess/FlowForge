#pragma once

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <utility>
#include <vector>

#include "handle.hpp"

namespace flwfrg::vk
{
class Instance;
class Surface;

struct PhysicalDeviceRequirements {
	bool graphics = true;
	bool present = true;
	bool compute = false;
	bool transfer = true;
	std::vector<const char *> device_extension_names{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	bool sampler_anisotropy = true;
	bool discrete_gpu = false;
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> present_modes{};
};

/// <summary>
/// Struct for storing the indices of queue families.
/// </summary>
struct QueueFamilyIndices {
	std::optional<uint32_t> graphics_family_index;
	std::optional<uint32_t> present_family_index;
	std::optional<uint32_t> compute_family_index;
	std::optional<uint32_t> transfer_family_index;

	/// Queue can do both graphics computation and presentation
	[[nodiscard]] constexpr bool is_complete() const noexcept
	{
		return graphics_family_index.has_value() && present_family_index.has_value();
	}

	[[nodiscard]] constexpr bool supports_graphics_computation() const noexcept
	{
		return graphics_family_index.has_value();
	}

	[[nodiscard]] constexpr bool supports_present() const noexcept
	{
		return present_family_index.has_value();
	}

	[[nodiscard]] constexpr bool supports_compute() const noexcept
	{
		return compute_family_index.has_value();
	}

	[[nodiscard]] constexpr bool supports_transfer() const noexcept
	{
		return transfer_family_index.has_value();
	}
};

class Device
{
public:
	Device(Instance *instance, Surface *surface, PhysicalDeviceRequirements requirements = {});
	~Device();

	[[nodiscard]] VkQueue get_present_queue() const { return present_queue_; };
	[[nodiscard]] VkQueue get_graphics_queue() const { return graphics_queue_; };
	[[nodiscard]] VkQueue get_transfer_queue() const { return transfer_queue_; };

	[[nodiscard]] uint32_t get_graphics_queue_index() const { return graphics_queue_index_.value(); };
	[[nodiscard]] uint32_t get_present_queue_index() const { return present_queue_index_.value(); };
	[[nodiscard]] uint32_t get_transfer_queue_index() const { return transfer_queue_index_.value(); };
	[[nodiscard]] uint32_t get_compute_queue_index() const { return compute_queue_index_.value(); };

	[[nodiscard]] bool has_graphics_queue() const { return graphics_queue_index_.has_value(); };
	[[nodiscard]] bool has_present_queue() const { return present_queue_index_.has_value(); };
	[[nodiscard]] bool has_transfer_queue() const { return transfer_queue_index_.has_value(); };
	[[nodiscard]] bool has_compute_queue() const { return compute_queue_index_.has_value(); };

	[[nodiscard]] VkDevice get_logical_device() const { return logical_device_; };

	[[nodiscard]] VkPhysicalDevice get_physical_device() const { return physical_device_; };
	[[nodiscard]] VkCommandPool get_graphics_command_pool() const { return graphics_command_pool_; };
	[[nodiscard]] VkPhysicalDeviceProperties get_physical_device_properties() const { return physical_device_properties_; };

private:
	Instance *instance_ = nullptr;
	Surface *surface_ = nullptr;

	PhysicalDeviceRequirements physical_device_requirements_{};

	VkPhysicalDevice physical_device_{};
	Handle<VkDevice> logical_device_{};
	SwapchainSupportDetails swapchain_support_{};

	std::optional<uint32_t> graphics_queue_index_;
	std::optional<uint32_t> present_queue_index_;
	std::optional<uint32_t> compute_queue_index_;
	std::optional<uint32_t> transfer_queue_index_;

	VkQueue graphics_queue_{};
	VkQueue present_queue_{};
	VkQueue transfer_queue_{};
	VkQueue compute_queue_{};

	Handle<VkCommandPool> graphics_command_pool_{};

	VkPhysicalDeviceProperties physical_device_properties_{};
	VkPhysicalDeviceFeatures features_{};
	VkPhysicalDeviceMemoryProperties memory_{};

	VkFormat depth_format_ = VK_FORMAT_UNDEFINED;


	///// Private methods

	void pick_physical_device();

	void create_logical_device();

	///// Helper methods

	[[nodiscard]] bool is_device_suitable(VkPhysicalDevice device);

	uint32_t rate_device_suitability(VkPhysicalDevice device);

	QueueFamilyIndices find_queue_families(VkPhysicalDevice device);

	SwapchainSupportDetails query_swapchain_support(VkPhysicalDevice device);

	inline SwapchainSupportDetails query_swapchain_support() { return query_swapchain_support(physical_device_); };

	[[nodiscard]] bool check_device_extension_support(VkPhysicalDevice device);

	bool detect_depth_format();
};

}// namespace flwfrg::vk