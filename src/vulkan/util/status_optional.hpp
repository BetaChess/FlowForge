#pragma once

#include <vulkan/vulkan.h>

#include <optional>

namespace flwfrg::vk
{
enum class Status
{
	SUCCESS,
	OUT_OF_HOST_MEMORY,
	OUT_OF_DEVICE_MEMORY,
	INVALID_SHADER,
	UNKNOWN_OR_INVALID_SHADER_STAGE,
	INVALID_OPAQUE_CAPTURE_ADDRESS,
	OUT_OF_DATE_KHR,
	SUBOPTIMAL_KHR,
	DEVICE_LOST,
	SURFACE_LOST_KHR,
	FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT,
    LAYER_NOT_PRESENT,
    NOT_READY,
    TIMEOUT,
    EVENT_SET,
    EVENT_RESET,
    INCOMPLETE,
    INITIALIZATION_FAILED,
    MEMORY_MAP_FAILED,
    EXTENSION_NOT_PRESENT,
    FEATURE_NOT_PRESENT,
    INCOMPATIBLE_DRIVER,
    TOO_MANY_OBJECTS,
    FORMAT_NOT_SUPPORTED,
    FRAGMENTED_POOL,
    OUT_OF_POOL_MEMORY,
    INVALID_EXTERNAL_HANDLE,
    FRAGMENTATION,
    PIPELINE_COMPILE_REQUIRED,
    NOT_PERMITTED,
    NATIVE_WINDOW_IN_USE_KHR,
    INCOMPATIBLE_DISPLAY_KHR,
    VALIDATION_FAILED_EXT,
    IMAGE_USAGE_NOT_SUPPORTED_KHR,
    VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR,
    VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR,
    VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR,
    VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR,
    VIDEO_STD_VERSION_NOT_SUPPORTED_KHR,
    INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT,
    THREAD_IDLE_KHR,
    THREAD_DONE_KHR,
    OPERATION_DEFERRED_KHR,
    OPERATION_NOT_DEFERRED_KHR,
    INVALID_VIDEO_STD_PARAMETERS_KHR,
    COMPRESSION_EXHAUSTED_EXT,
    INCOMPATIBLE_SHADER_BINARY_EXT,
    PIPELINE_BINARY_MISSING_KHR,
    NOT_ENOUGH_SPACE_KHR,
    // Flowforge specific errors
    FLOWFORGE_UNSUPPORTED_CHANNEL_COUNT,
    FLOWFORGE_FAILED_TO_OPEN_FILE,
    FLOWFORGE_FAILED_TO_LOAD_TEXTURE_DATA,
    UNKNOWN_ERROR,
};

constexpr Status vk_result_to_flwfrg_status(VkResult status)
{
   switch (status)
    {
    case VK_SUCCESS:
       return Status::SUCCESS;
   case VK_NOT_READY:
       return Status::NOT_READY;
    case VK_TIMEOUT:
        return Status::TIMEOUT;
    case VK_EVENT_SET:
        return Status::EVENT_SET;
    case VK_EVENT_RESET:
        return Status::EVENT_RESET;
    case VK_INCOMPLETE:
        return Status::INCOMPLETE;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return Status::OUT_OF_HOST_MEMORY;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return Status::OUT_OF_DEVICE_MEMORY;
    case VK_ERROR_INITIALIZATION_FAILED:
        return Status::INITIALIZATION_FAILED;
    case VK_ERROR_DEVICE_LOST:
        return Status::DEVICE_LOST;
    case VK_ERROR_MEMORY_MAP_FAILED:
        return Status::MEMORY_MAP_FAILED;
    case VK_ERROR_LAYER_NOT_PRESENT:
        return Status::LAYER_NOT_PRESENT;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return Status::EXTENSION_NOT_PRESENT;
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return Status::FEATURE_NOT_PRESENT;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return Status::INCOMPATIBLE_DRIVER;
    case VK_ERROR_TOO_MANY_OBJECTS:
        return Status::TOO_MANY_OBJECTS;
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return Status::FORMAT_NOT_SUPPORTED;
    case VK_ERROR_FRAGMENTED_POOL:
        return Status::FRAGMENTED_POOL;
    case VK_ERROR_UNKNOWN:
        return Status::UNKNOWN_ERROR;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return Status::OUT_OF_POOL_MEMORY;
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return Status::INVALID_EXTERNAL_HANDLE;
    case VK_ERROR_FRAGMENTATION:
        return Status::FRAGMENTATION;
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return Status::INVALID_OPAQUE_CAPTURE_ADDRESS;
    case VK_PIPELINE_COMPILE_REQUIRED:
        return Status::PIPELINE_COMPILE_REQUIRED;
    case VK_ERROR_NOT_PERMITTED:
        return Status::NOT_PERMITTED;
    case VK_ERROR_SURFACE_LOST_KHR:
        return Status::SURFACE_LOST_KHR;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return Status::NATIVE_WINDOW_IN_USE_KHR;
    case VK_SUBOPTIMAL_KHR:
        return Status::SUBOPTIMAL_KHR;
    case VK_ERROR_OUT_OF_DATE_KHR:
        return Status::OUT_OF_DATE_KHR;
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return Status::INCOMPATIBLE_DISPLAY_KHR;
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return Status::VALIDATION_FAILED_EXT;
    case VK_ERROR_INVALID_SHADER_NV:
        return Status::INVALID_SHADER;
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        return Status::IMAGE_USAGE_NOT_SUPPORTED_KHR;
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        return Status::VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR;
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        return Status::VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR;
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        return Status::VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR;
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        return Status::VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR;
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        return Status::VIDEO_STD_VERSION_NOT_SUPPORTED_KHR;
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return Status::INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT;
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return Status::FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT;
    case VK_THREAD_IDLE_KHR:
        return Status::THREAD_IDLE_KHR;
    case VK_THREAD_DONE_KHR:
        return Status::THREAD_DONE_KHR;
    case VK_OPERATION_DEFERRED_KHR:
        return Status::OPERATION_DEFERRED_KHR;
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return Status::OPERATION_NOT_DEFERRED_KHR;
    case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
        return Status::INVALID_VIDEO_STD_PARAMETERS_KHR;
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        return Status::COMPRESSION_EXHAUSTED_EXT;
    case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
        return Status::INCOMPATIBLE_SHADER_BINARY_EXT;
    case VK_PIPELINE_BINARY_MISSING_KHR:
        return Status::PIPELINE_BINARY_MISSING_KHR;
    case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
        return Status::NOT_ENOUGH_SPACE_KHR;
   default:
       // Should never be reached
       return Status::UNKNOWN_ERROR;
    }
}

template<typename T, typename STATUS_TYPE, STATUS_TYPE success_value>
class StatusOptional
{
public:
	StatusOptional() = default;
	StatusOptional(STATUS_TYPE error_code) : status_code_{error_code} {};
	inline StatusOptional(T &&obj) : optional_(std::move(obj)), status_code_{success_value} {};
	~StatusOptional() = default;

	// Copy
	StatusOptional(const StatusOptional &) = default;
	StatusOptional &operator=(const StatusOptional &) = default;
	// Move
	StatusOptional(StatusOptional &&other) noexcept = default;
	StatusOptional &operator=(StatusOptional &&other) noexcept = default;

	// Methods

	[[nodiscard]] constexpr bool has_value() const { return optional_.has_value(); }

	[[nodiscard]] constexpr const STATUS_TYPE &status() const noexcept { return status_code_; }

	[[nodiscard]] constexpr T &value() { return optional_.value(); }
	[[nodiscard]] constexpr const T &value() const { return optional_.value(); }

private:
	std::optional<T> optional_{};
	STATUS_TYPE status_code_;
};

}// namespace flwfrg::vk