#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace flwfrg
{

struct Transform
{
	glm::vec3 translation{};
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };

	[[nodiscard]] glm::mat4 mat4() const;
	[[nodiscard]] glm::mat3 normal_matrix() const;

	glm::vec3 rotation{};
};

}