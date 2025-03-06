#pragma once

#include "vulkan/descriptor.hpp"


#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

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

    ImGuiContext *get_context() const;

private:
    ImGuiContext *context_ = nullptr;
};

class IMGuiShader
{
public:
    struct Config
    {
        uint32_t texture_limit = 2;
        bool enable_docking = false;
    };

public:
	IMGuiShader() = default;
    IMGuiShader(DisplayContext* context);
	IMGuiShader(DisplayContext* context, Config config);
	~IMGuiShader();

	// Copy
	IMGuiShader(const IMGuiShader &) = delete;
	IMGuiShader &operator=(const IMGuiShader &) = delete;
	// Move
	IMGuiShader(IMGuiShader &&other) noexcept = default;
	IMGuiShader &operator=(IMGuiShader &&other) noexcept = default;

    ImGuiContext* get_context() const;

	void begin_frame();
	void end_frame(CommandBuffer& command_buffer);

private:
	DisplayContext *display_context_ = nullptr;

	DescriptorPool descriptor_pool_{};

	IMGuiInstance im_gui_instance_{};
};

}