#include "pch.hpp"

#include "keyboard_controller.hpp"

#include "math/transform.hpp"

namespace flwfrg
{

void KeyboardController::move_in_plane_XZ(GLFWwindow *window, Transform &transform, float dt) const
{
	// ROTATION
	glm::vec3 rotate{ 0.0f };

	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS)
		rotate.y += 1.0f;
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS)
		rotate.y -= 1.0f;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS)
		rotate.x += 1.0f;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS)
		rotate.x -= 1.0f;

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		transform.rotation += lookSpeed * dt * glm::normalize(rotate);

	transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
	transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

	// MOVEMENT
	float yaw = transform.rotation.y;
	const glm::vec3 forward{ glm::sin(yaw), 0.0f, glm::cos(yaw) };
	const glm::vec3 right{ forward.z, 0.0f, -forward.x };
	const glm::vec3 up{ 0.0f, -1.0f, 0.0f };

	glm::vec3 move{ 0.0f };
	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS)
		move += forward;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS)
		move -= forward;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS)
		move += right;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS)
		move -= right;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS)
		move += up;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS)
		move -= up;

	if (glm::dot(move, move) > std::numeric_limits<float>::epsilon())
		transform.translation += moveSpeed * dt * glm::normalize(move);
}
}// namespace flwfrg