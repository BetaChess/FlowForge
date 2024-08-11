
#include "vulkan/shader/vertex.hpp"


#include <../../src/default_shaders.hpp>

#include <iostream>

int main()
{
	flwfrg::init();
	flwfrg::vk::Renderer renderer{1000, 600, "TestName"};
	auto &display_context = renderer.get_display_context();
	flwfrg::vk::shader::IMGuiShader im_gui_shader(&renderer.get_display_context());
	flwfrg::vk::shader::MaterialShader material_shader(&renderer.get_display_context());

	std::array<flwfrg::vk::Vertex3d, 4> vertices{};
	vertices[0].position = {0.0, -0.5, 0, 0};
	vertices[1].position = {0.5, 0.5, 0, 0};
	vertices[2].position = {-0.5, 0.5, 0, 0};
	vertices[3].position = {0.0, 0.5, 0, 0};

	std::array<uint32_t, 4> indices{};
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	flwfrg::vk::Buffer vertex_buffer{
			&display_context.get_device(),
			sizeof(flwfrg::vk::Vertex3d) * vertices.size(),
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
	vertex_buffer.upload_data(
			vertices.data(), 0,
			sizeof(flwfrg::vk::Vertex3d) * vertices.size(),
			display_context.get_device().get_graphics_command_pool(),
			nullptr,
			display_context.get_device().get_graphics_queue());

	flwfrg::vk::Buffer index_buffer{
			&display_context.get_device(),
			sizeof(uint32_t) * vertices.size(),
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
	index_buffer.upload_data(
			indices.data(), 0,
			sizeof(uint32_t) * indices.size(),
			display_context.get_device().get_graphics_command_pool(),
			nullptr,
			display_context.get_device().get_graphics_queue());

	auto objid = material_shader.acquire_resources();


	while (!renderer.should_close())
	{
		auto frame_data = renderer.begin_frame();
		if (!frame_data.has_value())
			continue;

		im_gui_shader.begin_frame();

		material_shader.use();
		material_shader.update_global_state(1);
		flwfrg::vk::shader::GeometryRenderData data{};
		data.model = {1};
		data.object_id = objid;
		material_shader.update_object(data);

		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(frame_data.value()->get_handle(), 0, 1, vertex_buffer.ptr(), offsets);
		vkCmdBindIndexBuffer(frame_data.value()->get_handle(), index_buffer.get_handle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(frame_data.value()->get_handle(), indices.size(), 1, 0, 0, 0);

		ImGui::ShowDemoWindow();

		im_gui_shader.end_frame(*frame_data.value());

		renderer.end_frame();
	}

	material_shader.release_resources(objid);

	return 0;
}