#pragma once

#include "frame_buffer.hpp"
#include "image.hpp"
#include "util/status_optional.hpp"

#include <vulkan/vulkan_core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace flwfrg::vk
{
class DisplayContext;

class Swapchain
{
public:
    explicit Swapchain(DisplayContext *context);
    ~Swapchain();

    // Copy
    Swapchain(const Swapchain &) = delete;
    Swapchain &operator=(const Swapchain &) = delete;
    // Move
    Swapchain(Swapchain &&other) noexcept = default;
    Swapchain &operator=(Swapchain &&other) noexcept = default;

    // Methods
    Status acquire_next_image(uint64_t timeout_ns, VkSemaphore image_availiable_semaphore, VkFence fence,
                              uint32_t *out_image_index);
    Status present(VkQueue graphics_queue, VkQueue present_queue, VkSemaphore render_complete_semaphore,
                   uint32_t present_image_index);
    [[nodiscard]] inline uint8_t get_image_count() const { return swapchain_images_.size(); };
    [[nodiscard]] inline glm::vec2 get_frame_buffer_size() const { return frame_buffer_size_; };

private:
    DisplayContext *context_;

    VkSurfaceFormatKHR swapchain_image_format_;
    uint8_t max_frames_in_flight_ = 3;
    Handle<VkSwapchainKHR> swapchain_{};

    // Not stored in flwfrg::vk::Image since swapchain images are retrieved, not created.
    std::vector<VkImage> swapchain_images_;
    std::vector<VkImageView> swapchain_image_views_;

    std::unique_ptr<Image> depth_attachment_;

    glm::vec2 frame_buffer_size_ = {};
    std::vector<FrameBuffer> frame_buffers_;

    void recreate_swapchain();
    void regenerate_frame_buffers(RenderPass *renderpass);

    bool choose_swapchain_surface_format();

    friend DisplayContext;
};

} // namespace flwfrg::vk
