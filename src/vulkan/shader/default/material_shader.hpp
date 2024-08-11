#pragma once

#include "vulkan/buffer.hpp"
#include "vulkan/descriptor.hpp"
#include "vulkan/resource/texture.hpp"
#include "vulkan/shader/pipeline.hpp"
#include "vulkan/shader/shader_stage.hpp"
#include "material_object_types.inl"

namespace flwfrg::vk
{
class DisplayContext;
}

namespace flwfrg::vk::shader
{

class MaterialShader
{
public:
	GlobalUniformObject global_ubo{};

public:
	MaterialShader() = default;
	explicit MaterialShader(DisplayContext* context);
	~MaterialShader() = default;

	// Copy
	MaterialShader(const MaterialShader &) = delete;
	MaterialShader &operator=(const MaterialShader &) = delete;
	// Move
	MaterialShader(MaterialShader &&other) noexcept = default;
	MaterialShader &operator=(MaterialShader &&other) noexcept = default;

	// Methods

	[[nodiscard]] const DescriptorPool &get_global_descriptor_pool() const { return global_descriptor_pool_; };
	void update_global_state(float delta_time);
	void update_object(GeometryRenderData data);

	void use();

	[[nodiscard]] uint32_t acquire_resources();
	void release_resources(uint32_t object_id);

private:
	DisplayContext *context_ = nullptr;

	std::vector<ShaderStage> stages{};

	DescriptorPool global_descriptor_pool_{};
	DescriptorPool local_descriptor_pool_{};
	DescriptorSetLayout global_descriptor_set_layout_{};
	DescriptorSetLayout local_descriptor_set_layout_{};

	// One set per frame (max 3)
	std::array<VkDescriptorSet, 3> global_descriptor_sets_{};
	std::array<bool, 3> global_descriptor_updated_{};

	// Global uniform buffer
	Buffer global_uniform_buffer_{};

	// local object uniform buffer
	Buffer local_uniform_buffer_{};
	uint32_t object_uniform_buffer_index = 0; // Todo: manage a free list instead

	std::array<MaterialShaderObjectState, VULKAN_MATERIAL_SHADER_MAX_OBJECT_COUNT> object_states_{}; // Todo: Make dynamic later

	Pipeline pipeline_{};

	Texture default_texture_;

	// Static members

	static constexpr uint16_t shader_stage_count = 2;
	static constexpr const char *shader_file_name = "default_material_shader";
};

}