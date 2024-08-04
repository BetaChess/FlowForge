#pragma once

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
	INVALID_OPAQUE_CAPTURE_ADDRESS_KHR,
	OUT_OF_DATE_KHR,
	SUBOPTIMAL_KHR,
	DEVICE_LOST,
	SURFACE_LOST_KHR,
	FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT,
	FLOWFORGE_UNSUPPORTED_CHANNEL_COUNT,
	FLOWFORGE_FAILED_TO_OPEN_FILE,
	FLOWFORGE_FAILED_TO_LOAD_TEXTURE_DATA,
	UNKNOWN_ERROR,
};

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