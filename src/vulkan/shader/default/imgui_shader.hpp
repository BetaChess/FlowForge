#pragma once

#include "../../descriptor.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace flwfrg::vk
{
class Window;
class DisplayContext;
class CommandBuffer;
}

namespace flwfrg::vk::shader
{

class IMGuiInstance
{
public:
	IMGuiInstance() = default;
	IMGuiInstance(ImGui_ImplVulkan_InitInfo init_info, Window* window);
	~IMGuiInstance();

	// Copy
	IMGuiInstance(const IMGuiInstance &) = delete;
	IMGuiInstance &operator=(const IMGuiInstance &) = delete;
	// Move
	IMGuiInstance(IMGuiInstance &&other) noexcept;
	IMGuiInstance &operator=(IMGuiInstance &&other) noexcept;

private:
	bool context_created_ = false;
};

class IMGuiShader
{
public:
	IMGuiShader() = default;
	IMGuiShader(DisplayContext* context);
	~IMGuiShader();

	// Copy
	IMGuiShader(const IMGuiShader &) = delete;
	IMGuiShader &operator=(const IMGuiShader &) = delete;
	// Move
	IMGuiShader(IMGuiShader &&other) noexcept = default;
	IMGuiShader &operator=(IMGuiShader &&other) noexcept = default;

	void begin_frame();
	void end_frame(CommandBuffer& command_buffer);

private:
	DisplayContext *display_context_ = nullptr;

	DescriptorPool descriptor_pool_{};

	IMGuiInstance im_gui_instance_{};
};

}