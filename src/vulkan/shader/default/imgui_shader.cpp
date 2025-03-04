#include "pch.hpp"

#include "imgui_shader.hpp"

#include "vulkan/display_context.hpp"
#include "vulkan/window.hpp"

#include <imgui_impl_glfw.h>

namespace flwfrg::vk::shader
{


IMGuiInstance::IMGuiInstance(ImGui_ImplVulkan_InitInfo init_info, Window *window)
	: context_created_{true}
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Init glfw for imgui
	ImGui_ImplGlfw_InitForVulkan(window->get_glfw_window_ptr(), true);

	ImGui_ImplVulkan_Init(&init_info);
}
IMGuiInstance::~IMGuiInstance()
{
	if (context_created_)
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
IMGuiInstance::IMGuiInstance(IMGuiInstance &&other) noexcept
	: context_created_(other.context_created_)
{
	other.context_created_ = false;
}

IMGuiInstance &IMGuiInstance::operator=(IMGuiInstance &&other) noexcept
{
	if (this != &other)
	{
		context_created_ = other.context_created_;
		other.context_created_ = false;
	}
	return *this;
}

IMGuiShader::IMGuiShader(DisplayContext *context, uint32_t texture_limit)
	: display_context_{context}
{
	assert(display_context_ != nullptr);
	// // Global descriptor pool
	// VkDescriptorPoolSize global_pool_size{};
	// global_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// global_pool_size.descriptorCount = context->get_swapchain().get_image_count();

	// Image sampler pool
	VkDescriptorPoolSize image_sampler_pool_size{};
	image_sampler_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	image_sampler_pool_size.descriptorCount = texture_limit;

	std::vector<VkDescriptorPoolSize> pool_sizes = {
			// global_pool_size,
			image_sampler_pool_size};

	VkDescriptorPoolCreateInfo global_pool_info{};
	global_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	global_pool_info.poolSizeCount = pool_sizes.size();
	global_pool_info.pPoolSizes = pool_sizes.data();
	global_pool_info.maxSets = context->get_swapchain().get_image_count() + 1;
	global_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	descriptor_pool_ = DescriptorPool(&context->get_device(), global_pool_info);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = context->get_instance().handle();
	init_info.PhysicalDevice = context->get_device().get_physical_device();
	init_info.Device = context->get_device().get_logical_device();
	init_info.QueueFamily = context->get_device().get_graphics_queue_index();
	init_info.Queue = context->get_device().get_graphics_queue();
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = descriptor_pool_.handle();
	init_info.Subpass = 0;
	init_info.MinImageCount = context->get_swapchain().get_image_count();
	init_info.ImageCount = context->get_swapchain().get_image_count();
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	init_info.CheckVkResultFn = nullptr;
	init_info.RenderPass = context->get_main_render_pass().handle();

	im_gui_instance_ = std::move(IMGuiInstance(init_info, context->get_window()));
}
IMGuiShader::~IMGuiShader()
{
	vkDeviceWaitIdle(display_context_->get_device().get_logical_device());
}

void IMGuiShader::begin_frame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void IMGuiShader::end_frame(CommandBuffer &command_buffer)
{
	// ImGui rendering
	ImGui::Render();
	ImDrawData *main_draw_data = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(main_draw_data, command_buffer.get_handle());
}
}// namespace flwfrg::vk::shader