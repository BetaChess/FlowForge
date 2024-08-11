#pragma once

#include "vulkan/shader/pipeline.hpp"
#include "vulkan/shader/shader_stage.hpp"

namespace flwfrg::vk
{
class DisplayContext;
}

namespace flwfrg::vk::shader
{

class SimpleShader
{
public:
	struct Vertex {
		glm::vec3 position{0};
		alignas(sizeof(glm::vec4)) glm::vec4 color{1};

		static inline std::vector<VkVertexInputAttributeDescription> get_binding_description()
		{
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

			attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
			attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)});

			return attributeDescriptions;
		}
	};

public:
	SimpleShader() = default;
	explicit SimpleShader(DisplayContext *context);
	~SimpleShader() = default;

	// Copy
	SimpleShader(const SimpleShader &) = delete;
	SimpleShader &operator=(const SimpleShader &) = delete;
	// Move
	SimpleShader(SimpleShader &&other) noexcept = default;
	SimpleShader &operator=(SimpleShader &&other) noexcept = default;

	// Methods

	void use();

private:
	DisplayContext *context_ = nullptr;

	std::vector<ShaderStage> stages_{};

	Pipeline pipeline_{};

	// Static members

	static constexpr uint16_t shader_stage_count = 2;
	static constexpr const char *shader_file_name = "default_simple_shader";
};

}// namespace flwfrg::vk::shader