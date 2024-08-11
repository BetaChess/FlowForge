
#include <../../src/default_shaders.hpp>

#include <iostream>

int main()
{
	flwfrg::init();
	flwfrg::vk::Renderer renderer{1000, 600, "ImGUI Demo"};
	flwfrg::vk::shader::IMGuiShader im_gui_shader(&renderer.get_display_context());

	while (!renderer.should_close())
	{
		auto frame_data = renderer.begin_frame();
		if (!frame_data.has_value())
			continue;

		im_gui_shader.begin_frame();

		ImGui::ShowDemoWindow();

		im_gui_shader.end_frame(*frame_data.value());

		renderer.end_frame();
	}

	return 0;
}