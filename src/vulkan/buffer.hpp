#pragma once

#include "util/handle.hpp"

namespace flwfrg::vk
{
class Device;

class Buffer
{
public:
	Buffer() = default;
	Buffer(Device* device,
				 uint64_t size,
				 VkBufferUsageFlagBits usage,
				 uint32_t memory_property_flags,
				 bool bind_on_create);
	~Buffer();

	// Copy
	Buffer(const Buffer &) = delete;
	Buffer &operator=(const Buffer &) = delete;
	// Move
	Buffer(Buffer &&other) noexcept = default;
	Buffer &operator=(Buffer &&other) noexcept = default;

	// Methods

	[[nodiscard]] inline VkBuffer get_handle() const { return handle_; };
	[[nodiscard]] inline VkBuffer* ptr() { return handle_.ptr(); };
    [[nodiscard]] inline uint64_t get_total_size() const { return total_size_; }

	void resize(uint64_t new_size, VkQueue queue, VkCommandPool pool);

	void *lock_memory(uint64_t offset, uint64_t size, uint32_t flags);
	void unlock_memory();

	void bind(uint64_t offset);

	/// Loads data into the buffer, provided the buffer is host visible. Else throws an exception.
	/// @param data Data to be loaded into the buffer object
	/// @param offset Offset to load the data into
	/// @param size The amount of data to upload (in bytes)
	/// @param flags Additional memory mapping flags (Specified with VkMemoryMapFlagBits)
	void load_data(const void *data, uint64_t offset, uint64_t size, uint32_t flags);

	/// Loads data into the buffer using a staging buffer. The buffer is therefore not required to be host visible.
	/// This method is generally not desirable to use, since it creates a temporary buffer on each use.
	/// This may however be fine for some use-cases, which is why the option is provided.
	/// @param data Data to be loaded into the buffer object
	/// @param offset Offset to load the data into
	/// @param size The amount of data to upload (in bytes)
	void upload_data(const void* data, uint64_t offset, uint64_t size, VkCommandPool pool, VkFence fence, VkQueue queue);

	void copy_to(Buffer &dst,
				 uint64_t dst_offset,
				 uint64_t dst_size,
				 uint64_t src_offset,
				 VkCommandPool pool,
				 VkFence fence,
				 VkQueue queue);

private:
	Device* device_ = nullptr;

	uint64_t total_size_ = 0;
	Handle<VkBuffer> handle_{};
	VkBufferUsageFlagBits usage_ = static_cast<VkBufferUsageFlagBits>(0);
	bool locked_ = false;
	Handle<VkDeviceMemory> memory_{};
	int32_t memory_index_ = -1;
	uint32_t memory_property_flags_ = 0;
};



}
