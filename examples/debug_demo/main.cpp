
#include "default_shaders.hpp"
#include "input/keyboard_controller.hpp"
#include "math/camera.hpp"
#include "math/transform.hpp"
#include "vulkan/shader/vertex.hpp"

int main()
{
    flwfrg::init();
    flwfrg::vk::Renderer renderer{600, 600, "Debug Demo", flwfrg::vk::shader::DebugShader::get_minimum_requirements()};
    auto &display_context = renderer.get_display_context();
    flwfrg::vk::shader::DebugShader debug_shader(&renderer.get_display_context());

    std::array<flwfrg::vk::ColorVertex, 4> vertices{};
    vertices[0].position = {-0.5, 0.5, 0};
    vertices[0].color = {1.0, 0.0, 0.0, 1.0};
    vertices[1].position = {0.5, -0.5, 0};
    vertices[1].color = {0.0, 1.0, 0.0, 1.0};
    vertices[2].position = {-0.5, -0.5, 0};
    vertices[2].color = {0.0, 0.0, 1.0, 1.0};
    vertices[3].position = {0.5, 0.5, 0};

    std::array<uint32_t, 6> indices{};
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 0;
    indices[4] = 3;
    indices[5] = 1;

    flwfrg::vk::Buffer vertex_buffer{&display_context.get_device(), sizeof(flwfrg::vk::ColorVertex) * vertices.size(),
                                     static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
    vertex_buffer.upload_data(vertices.data(), 0, sizeof(flwfrg::vk::ColorVertex) * vertices.size(),
                              display_context.get_device().get_graphics_command_pool(), nullptr,
                              display_context.get_device().get_graphics_queue());

    flwfrg::vk::Buffer index_buffer{&display_context.get_device(), sizeof(uint32_t) * indices.size(),
                                    static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                                                       VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
    index_buffer.upload_data(indices.data(), 0, sizeof(uint32_t) * indices.size(),
                             display_context.get_device().get_graphics_command_pool(), nullptr,
                             display_context.get_device().get_graphics_queue());

    flwfrg::vk::ColorModelManager::GeometryRenderData object_data{};

    flwfrg::KeyboardController controller;
    flwfrg::Camera camera;
    flwfrg::Transform camera_transform;
    camera.set_perspective_projection(glm::radians(50.0f), 1200.0f / 720.0f, 0.1f, 1000.0f);
    camera_transform.translation.z = -10;

    flwfrg::Transform object_transform;

    int64_t cumulative_time = 0;

    while (!renderer.should_close())
    {
        if (cumulative_time > 3000)
        {
            cumulative_time = 0;
            debug_shader.use_wire_frame(!debug_shader.using_wire_frame());
        }

        auto frame_start_time = std::chrono::high_resolution_clock::now();

        auto frame_data = renderer.begin_frame();
        if (!frame_data.has_value())
            continue;

        controller.move_in_plane_XZ(display_context.get_window()->get_glfw_window_ptr(), camera_transform, 0.007);
        camera.set_viewYXZ(camera_transform.translation, camera_transform.rotation);
        debug_shader.update_global_state(camera.get_projection(), camera.get_view());
        object_data.model = object_transform.mat4();
        debug_shader.update_object(object_data);

        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(frame_data.value()->get_handle(), 0, 1, vertex_buffer.ptr(), offsets);
        vkCmdBindIndexBuffer(frame_data.value()->get_handle(), index_buffer.get_handle(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(frame_data.value()->get_handle(), indices.size(), 1, 0, 0, 0);


        renderer.end_frame();

        auto frame_end_time = std::chrono::high_resolution_clock::now();
        cumulative_time += std::chrono::duration_cast<std::chrono::milliseconds>(frame_end_time - frame_start_time).count();
    }

    vkDeviceWaitIdle(display_context.get_device().get_logical_device());

    return 0;
}
