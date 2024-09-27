
#include "default_shaders.hpp"

int main()
{
	flwfrg::init();
	flwfrg::vk::Renderer renderer{600, 600, "Simple Square Demo"};
	auto &display_context = renderer.get_display_context();
	flwfrg::vk::shader::SimpleShader simple_shader(&renderer.get_display_context());

	std::array<flwfrg::vk::shader::SimpleShader::Vertex, 4> vertices{};
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

	flwfrg::vk::Buffer vertex_buffer{
			&display_context.get_device(),
			sizeof(flwfrg::vk::shader::SimpleShader::Vertex) * vertices.size(),
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
	vertex_buffer.upload_data(
			vertices.data(), 0,
			sizeof(flwfrg::vk::shader::SimpleShader::Vertex) * vertices.size(),
			display_context.get_device().get_graphics_command_pool(),
			nullptr,
			display_context.get_device().get_graphics_queue());

	flwfrg::vk::Buffer index_buffer{
			&display_context.get_device(),
			sizeof(uint32_t) * indices.size(),
			static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false};
	index_buffer.upload_data(
			indices.data(), 0,
			sizeof(uint32_t) * indices.size(),
			display_context.get_device().get_graphics_command_pool(),
			nullptr,
			display_context.get_device().get_graphics_queue());

	while (!renderer.should_close())
	{
		auto frame_data = renderer.begin_frame();
		if (!frame_data.has_value())
			continue;

		simple_shader.use();
		VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(frame_data.value()->get_handle(), 0, 1, vertex_buffer.ptr(), offsets);
		vkCmdBindIndexBuffer(frame_data.value()->get_handle(), index_buffer.get_handle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(frame_data.value()->get_handle(), indices.size(), 1, 0, 0, 0);


		renderer.end_frame();
	}

	vkDeviceWaitIdle(display_context.get_device().get_logical_device());

	return 0;
}