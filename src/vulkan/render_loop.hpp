#pragma once

#include "util/status_optional.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{

class RenderLoop
{
public:
    virtual ~RenderLoop() = default;

    virtual Status acquire_next_image(uint64_t timeout_ns, VkSemaphore image_available_semaphore, VkFence fence,
                              uint32_t *out_image_index) = 0;
    [[nodiscard]] virtual uint8_t get_image_count() const = 0;
    [[nodiscard]] virtual glm::vec2 get_frame_buffer_size() const = 0;
};

}
