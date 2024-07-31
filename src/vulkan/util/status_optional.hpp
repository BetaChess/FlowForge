#pragma once

#include <optional>

namespace flwfrg::vk
{
template<typename T, typename STATUS_TYPE, STATUS_TYPE success_value>
class StatusOptional
{
public:
	StatusOptional() = default;
	StatusOptional(STATUS_TYPE error_code) : status_code_{error_code} {};
	inline StatusOptional(T obj) : optional_{obj}, status_code_{success_value} {};
	~StatusOptional() = default;

	// Copy
	StatusOptional(const StatusOptional &) = default;
	StatusOptional &operator=(const StatusOptional &) = default;
	// Move
	StatusOptional(StatusOptional &&other) noexcept = default;
	StatusOptional &operator=(StatusOptional &&other) noexcept = default;

	// Methods

	[[nodiscard]] constexpr bool has_value() const { return optional_.has_value(); }

	[[nodiscard]] constexpr const STATUS_TYPE& status() const noexcept { return status_code_; }

	[[nodiscard]] constexpr T& value() { return optional_.value(); }
	[[nodiscard]] constexpr const T& value() const { return optional_.value(); }

private:
	std::optional<T> optional_{};
	STATUS_TYPE status_code_;
};

}