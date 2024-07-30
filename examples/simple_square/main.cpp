
#include <flowforge.hpp>
#include <iostream>

int main()
{
	flwfrg::init();
	flwfrg::GLFWContext glfw_context;
	flwfrg::vk::Window window{600, 400, "Test"};
	flwfrg::vk::DisplayContext display_context{&window};

	while (!window.should_close())
	{
		glfwPollEvents();
	}

	return 0;
}