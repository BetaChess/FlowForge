
#include <../../src/default_shaders.hpp>

#include <iostream>

#include "vulkan/resource/im_gui_texture.hpp"

int main()
{
    flwfrg::init();
    flwfrg::vk::Renderer renderer{1000, 600, "ImGUI Demo"};
    flwfrg::vk::shader::IMGuiShader im_gui_shader(&renderer.get_display_context());

    flwfrg::vk::ImGuiTexture texture = std::move(
            flwfrg::vk::ImGuiTexture::create_imgui_texture(&renderer.get_display_context().get_device(), 500, 500, 4)
                    .value());

    while (!renderer.should_close())
    {
        auto frame_data = renderer.begin_frame();
        if (!frame_data.has_value())
            continue;

        im_gui_shader.begin_frame();

        ImGui::ShowDemoWindow();


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Texture");

        ImGui::Image((ImTextureID)texture.get_descriptor_set(), {(float)texture.get_width(), (float)texture.get_height()},
                     ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();

        im_gui_shader.end_frame(*frame_data.value());

        renderer.end_frame();
    }

    return 0;
}
