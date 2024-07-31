#pragma once

#include "util/handle.hpp"


#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{
class Device;

class Fence
{
public:
	explicit Fence(Device* device, bool signaled = true);
	~Fence();

	// Copy
	Fence(const Fence &) = delete;
	Fence &operator=(const Fence &) = delete;
	// Move
	Fence(Fence &&other) noexcept = default;
	Fence &operator=(Fence &&other) noexcept = default;

	[[nodiscard]] inline VkFence get_handle() const { return handle_; }
	[[nodiscard]] inline bool is_signaled() const { return signaled_; }
	bool wait(uint64_t timeout_ns);
	void reset();

private:
	Device* device_;

	Handle<VkFence> handle_{};
	bool signaled_ = true;
};

}
