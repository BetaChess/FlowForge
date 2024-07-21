
#include <flowforge.hpp>
#include <iostream>

int main()
{
	flwfrg::init();

	flwfrg::GLFWContext glfw_context;
	flwfrg::vk::Window window{600, 400, "Test"};
	flwfrg::vk::Instance instance;
	flwfrg::vk::PhysicalDeviceRequirements requirements;
	requirements.present = false;
	requirements.device_extension_names.clear();
	flwfrg::vk::Surface surface{&instance, &window};
	flwfrg::vk::Device device{&instance, &surface, requirements};

	while (!window.should_close())
	{
		glfwPollEvents();
	}

	return 0;
}