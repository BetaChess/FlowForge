#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <vulkan/vulkan_core.h>

namespace flwfrg::vk
{

struct Vertex3d
{
	glm::vec3 position{0};
	// glm::vec2 texture_coordinate{0};

	static inline std::vector<VkVertexInputAttributeDescription> get_binding_description()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex3d, position)});
		// attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex3d, texture_coordinate)});

		return attributeDescriptions;
	}

};

}