#pragma once

#include "material_object_types.inl"
#include "vulkan/buffer.hpp"
#include "vulkan/descriptor.hpp"
#include "vulkan/resource/static_texture.hpp"
#include "vulkan/resource/texture.hpp"
#include "vulkan/shader/pipeline.hpp"
#include "vulkan/shader/shader_stage.hpp"

namespace flwfrg::vk
{
class DisplayContext;
}

namespace flwfrg::vk::shader
{

class MaterialShader
{
public:
	struct Vertex {
		glm::vec3 position{0};
		alignas(glm::vec4) glm::vec2 texture_coordinate;
		// alignas(sizeof(glm::vec4)) glm::vec4 color{1};

		static inline std::vector<VkVertexInputAttributeDescription> get_binding_description()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
			attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texture_coordinate)});
			// attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)});

			return attributeDescriptions;
		}
	};

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

	// [[nodiscard]] const DescriptorPool &get_global_descriptor_pool() const { return global_descriptor_pool_; };
	[[nodiscard]] const Texture* get_default_texture() const { return &default_texture_; };

	void update_global_state(glm::mat4 projection, glm::mat4 view);
	void update_object(GeometryRenderData data);

	void use();

	[[nodiscard]] uint32_t acquire_resources();
	void release_resources(uint32_t object_id);

private:
	DisplayContext *context_ = nullptr;

	std::vector<ShaderStage> stages_{};

	DescriptorPool global_descriptor_pool_{};
	DescriptorSetLayout global_descriptor_set_layout_{};
	DescriptorPool local_descriptor_pool_{};
	DescriptorSetLayout local_descriptor_set_layout_{};

	// One set per frame (max 3)
	std::array<VkDescriptorSet, 3> global_descriptor_sets_{};
	std::array<bool, 3> global_descriptor_updated_{};

	// Global uniform buffer
	GlobalUniformObject global_ubo{};
	Buffer global_uniform_buffer_{};

	// local object uniform buffer
	Buffer local_uniform_buffer_{};
	uint32_t object_uniform_buffer_index = 0; // Todo: manage a free list instead

	std::array<MaterialShaderObjectState, VULKAN_MATERIAL_SHADER_MAX_OBJECT_COUNT> object_states_{}; // Todo: Make dynamic later

	Pipeline pipeline_{};

	StaticTexture default_texture_;

	// Static members

	static constexpr uint16_t shader_stage_count = 2;
	static constexpr const char *shader_file_name = "default_material_shader";
};

}