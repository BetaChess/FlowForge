#pragma once

#include <vulkan/vulkan.h>


namespace flwfrg::vk
{

template<typename HANDLE_T>
class Handle
{
public:
	constexpr Handle() = default;
	constexpr explicit Handle(HANDLE_T handle) noexcept : handle_{handle} {}
	// Copy
	Handle(const Handle &) = delete;
	Handle &operator=(const Handle &) = delete;
	// Move
	constexpr Handle(Handle &&other) noexcept : handle_{other.handle_}
	{
		other.handle_ = VK_NULL_HANDLE;
	}
	constexpr Handle &operator=(Handle &&other) noexcept
	{
		handle_ = other.handle_;
		other.handle_ = VK_NULL_HANDLE;

		return *this;
	}

	constexpr operator HANDLE_T() const noexcept
	{
		return handle_;
	}

	constexpr HANDLE_T* ptr() noexcept
	{
		return &handle_;
	}

	constexpr HANDLE_T* ptr() const noexcept
	{
		return &handle_;
	}

	[[nodiscard]] constexpr bool not_null() const noexcept
	{
		return handle_ != VK_NULL_HANDLE;
	}


protected:
	HANDLE_T handle_ = VK_NULL_HANDLE;
};

template<typename HANDLE_T>
constexpr Handle<HANDLE_T> make_handle(HANDLE_T handle)
{
	return Handle<HANDLE_T>(handle);
}

}// namespace flwfrg::vk
