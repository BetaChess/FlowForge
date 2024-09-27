#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vulkan/vulkan.h>

#include <array>

namespace flwfrg::vk
{
class Texture;
}

namespace flwfrg::vk::shader
{


struct DescriptorState
{
	std::array<uint32_t, 3> generations{};
};

#define VULKAN_MATERIAL_SHADER_DESCRIPTOR_COUNT 2
#define VULKAN_MATERIAL_SHADER_MAX_OBJECT_COUNT 1024

struct MaterialShaderObjectState
{
	std::array<VkDescriptorSet, 3> descriptor_sets{};

	std::array<DescriptorState, VULKAN_MATERIAL_SHADER_DESCRIPTOR_COUNT> descriptor_states{};
};

struct GeometryRenderData
{
	// uint32_t object_id;
	glm::mat4 model;
	// std::array<Texture*, 16> textures;
};

struct GlobalUniformObject
{
	glm::mat4 projection;	// 64 bytes
	glm::mat4 view;			// 64 bytes
	glm::mat4 _reserved0;	// 64 bytes
	glm::mat4 _reserved1;	// 64 bytes
};

struct LocalUniformObject
{
	glm::vec4 diffuse_color = {1.0f, 1.0f, 1.0f, 1.0f};
	glm::vec4 _reserved0;
	glm::vec4 _reserved1;
	glm::vec4 _reserved2;
};

}