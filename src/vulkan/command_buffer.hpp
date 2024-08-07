#pragma once
#include "util/handle.hpp"

namespace flwfrg::vk
{
class Device;
class RenderPass;

class CommandBuffer
{
public:
	enum class State
	{
		READY,
		RECORDING,
		IN_RENDER_PASS,
		RECORDING_ENDED,
		SUBMITTED,
		NOT_ALLOCATED
	};

public:
	CommandBuffer(Device *device, VkCommandPool pool, bool is_primary);
	~CommandBuffer();

	// Copy
	CommandBuffer(const CommandBuffer &) = delete;
	CommandBuffer &operator=(const CommandBuffer &) = delete;
	// Move
	constexpr CommandBuffer(CommandBuffer &&other) noexcept
		: device_{std::move(other.device_)},
		  handle_{std::move(other.handle_)},
		  pool_handle_{std::move(other.pool_handle_)},
		  state_{std::move(other.state_)}
	{
		other.state_ = State::NOT_ALLOCATED;
		other.handle_ = Handle<VkCommandBuffer>{};
		other.pool_handle_ = VK_NULL_HANDLE;
		other.device_ = nullptr;
	}

	constexpr CommandBuffer &operator=(CommandBuffer &&other) noexcept
	{
		if (this != &other)
		{
			device_ = std::move(other.device_);
			handle_ = std::move(other.handle_);
			pool_handle_ = std::move(other.pool_handle_);
			state_ = std::move(other.state_);

			other.state_ = State::NOT_ALLOCATED;
			other.handle_ = Handle<VkCommandBuffer>{};
			other.pool_handle_ = VK_NULL_HANDLE;
			other.device_ = nullptr;
		}
		return *this;
	}

	// Methods
	[[nodiscard]] inline VkCommandBuffer get_handle() noexcept { return handle_; };

	void begin(bool is_single_use, bool is_renderpass_continue, bool is_simultaneous_use);
	void end();
	void submit(VkQueue queue, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore, VkFence fence, VkPipelineStageFlags *flags = nullptr);
	void reset();

	// Static methods
	static CommandBuffer begin_single_time_commands(Device *device, VkCommandPool pool);
	static void end_single_time_commands(Device *device, CommandBuffer &command_buffer, VkQueue queue);

private:
	Device *device_ = nullptr;

	Handle<VkCommandBuffer> handle_{};
	VkCommandPool pool_handle_{VK_NULL_HANDLE};

	State state_{State::NOT_ALLOCATED};

	friend RenderPass;
};

}// namespace flwfrg::vk