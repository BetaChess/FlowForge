#pragma once

#include "material_object_types.inl"
#include "vulkan/buffer.hpp"
#include "vulkan/device.hpp"
#include "vulkan/resource/model_manager.hpp"
#include "vulkan/shader/pipeline.hpp"
#include "vulkan/shader/shader_stage.hpp"

namespace flwfrg::vk
{
class DisplayContext;
}

namespace flwfrg::vk::shader
{

class DebugShader
{
public:
    DebugShader() = default;
    explicit DebugShader(DisplayContext *context);
    ~DebugShader() = default;

    // Copy
    DebugShader(const DebugShader &) = delete;
    DebugShader &operator=(const DebugShader &) = delete;
    // Move
    DebugShader(DebugShader &&other) noexcept = default;
    DebugShader &operator=(DebugShader &&other) noexcept = default;

    // Methods
    void update_global_state(glm::mat4 projection, glm::mat4 view);
    void update_object(ColorModelManager::GeometryRenderData data);

    void use();

    inline void use_wire_frame(bool value) { using_wire_frame_ = value; };
    inline bool using_wire_frame() const { return using_wire_frame_; }

    static inline PhysicalDeviceRequirements get_minimum_requirements()
    {
        VkPhysicalDeviceFeatures features{};
        features.fillModeNonSolid = VK_TRUE;
        return PhysicalDeviceRequirements{
            .graphics = true,
            .present = true,
            .transfer = true,
            .device_extension_names = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
            .required_features = features,
        };
    }

private:
    DisplayContext *context_ = nullptr;

    std::vector<ShaderStage> stages_{};

    DescriptorPool global_descriptor_pool_{};
    DescriptorSetLayout global_descriptor_set_layout_{};

    // One set per frame (max 3)
    std::array<VkDescriptorSet, 3> global_descriptor_sets_{};
    std::array<bool, 3> global_descriptor_updated_{};

    // Global uniform buffer
    GlobalUniformObject global_ubo{};
    Buffer global_uniform_buffer_{};

    bool using_wire_frame_ = true;
    Pipeline pipeline_wire_frame_{};
    Pipeline pipeline_solid_{};

    // Static members

    static constexpr uint16_t shader_stage_count = 2;
    static constexpr const char *shader_file_name = "default_debug_shader";
};

} // namespace flwfrg::vk::shader
