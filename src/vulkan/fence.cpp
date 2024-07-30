#include "pch.hpp"

#include "fence.hpp"

#include "device.hpp"


namespace flwfrg::vk
{


Fence::Fence(Device *device, bool signaled)
	: device_{device}, signaled_{signaled}
{
	assert(device_ != nullptr);

	VkFenceCreateInfo fence_info = {};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = signaled_ ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	if (vkCreateFence(device_->get_logical_device(), &fence_info, nullptr, handle_.ptr()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create fence");
	}
	FLOWFORGE_TRACE("Fence created");
}

Fence::~Fence()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyFence(device_->get_logical_device(), handle_, nullptr);
		FLOWFORGE_TRACE("Fence destroyed");
	}
}

bool Fence::wait(uint64_t timeout_ns)
{
	if (signaled_)
	{
		return true;
	}

	// Wait and check result
	VkResult result = vkWaitForFences(device_->get_logical_device(), 1, handle_.ptr(), VK_TRUE, timeout_ns);

	switch (result)
	{
		case VK_SUCCESS:
			signaled_ = true;
			return true;
		case VK_TIMEOUT:
			FLOWFORGE_WARN("Fence wait timed out");
			return false;
		case VK_ERROR_DEVICE_LOST:
			FLOWFORGE_ERROR("Device lost while waiting for fence");
			return false;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			FLOWFORGE_ERROR("Out of host memory while waiting for fence");
			return false;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			FLOWFORGE_ERROR("Out of device memory while waiting for fence");
			return false;
		default:
			FLOWFORGE_ERROR("Failed to wait for fence. Unkown Error occurred. ");
			return false;
	}
}

void Fence::reset()
{
	if (signaled_)
	{
		// Reset fence and check result
		if (vkResetFences(device_->get_logical_device(), 1, handle_.ptr()) != VK_SUCCESS)
		{
			FLOWFORGE_ERROR("Failed to reset fence");
			return;
		}

		signaled_ = false;
	}
}


}// namespace flwfrg::vk
