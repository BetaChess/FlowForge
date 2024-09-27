#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace flwfrg
{

class Camera
{
public:

	void set_orthographic_projection(float left, float right, float top, float bottom, float near, float far);
	void set_perspective_projection(float fovy, float aspect, float near, float far);

	void set_view_direction(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
	void set_view_target(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f));
	void set_viewYXZ(glm::vec3 position, glm::vec3 rotation);

	[[nodiscard]] inline const glm::mat4& get_projection() const { return projection_matrix_; };
	[[nodiscard]] inline const glm::mat4& get_view() const { return view_matrix_; };
	[[nodiscard]] inline const glm::mat4& get_inverseView() const { return inverse_view_matrix_; };

private:
	glm::mat4 projection_matrix_{ 1.0f };
	glm::mat4 view_matrix_{ 1.0f };
	glm::mat4 inverse_view_matrix_{ 1.0f };
};

}// namespace flwfrg