#pragma once

#include "GLFW/glfw3.h"

namespace flwfrg
{

struct Transform;

class KeyboardController
{
public:
	struct KeyMappings
	{
		int moveLeft = GLFW_KEY_A;
		int moveRight = GLFW_KEY_D;
		int moveForward = GLFW_KEY_W;
		int moveBackward = GLFW_KEY_S;
		int moveUp = GLFW_KEY_SPACE;
		int moveDown = GLFW_KEY_LEFT_SHIFT;
		int lookRight = GLFW_KEY_RIGHT;
		int lookLeft = GLFW_KEY_LEFT;
		int lookUp = GLFW_KEY_UP;
		int lookDown = GLFW_KEY_DOWN;
	};

	void move_in_plane_XZ(GLFWwindow* window, Transform& transform, float dt) const;

	KeyMappings keys{};
	float moveSpeed{ 15.0f };
	float lookSpeed{ 1.5f };
};

}