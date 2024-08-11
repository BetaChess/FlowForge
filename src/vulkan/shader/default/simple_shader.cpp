#include "pch.hpp"

#include "simple_shader.hpp"

#include "vulkan/display_context.hpp"
#include "vulkan/shader/vertex.hpp"

namespace flwfrg::vk::shader
{


SimpleShader::SimpleShader(DisplayContext *context)
	: context_{context}
{
	assert(context_ != nullptr);

	VkShaderStageFlagBits stage_types[shader_stage_count] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

	// Iterate over each stage
	for (auto &stage_type: stage_types)
	{
		// Create a shader stage
		auto stage = ShaderStage::create_shader_module(&context_->get_device(), shader_file_name, stage_type);

		if (!stage.has_value())
		{
			throw std::runtime_error("Failed to create shader stage");
		}

		stages_.emplace_back(std::move(stage.value()));
	}


	// Pipeline creation
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(context_->get_window()->get_width());
	viewport.height = static_cast<float>(context_->get_window()->get_height());
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = {context_->get_window()->get_width(), context_->get_window()->get_height()};

	// Attributes
	auto binding_description = SimpleShader::Vertex::get_binding_description();

	// Stages
	std::vector<VkPipelineShaderStageCreateInfo> stage_create_infos{};
	stage_create_infos.resize(shader_stage_count);
	for (uint16_t i = 0; i < shader_stage_count; i++)
	{
		stage_create_infos[i] = stages_[i].get_shader_stage_create_info();
	}

	Pipeline::PipelineConfig pipeline_config{};
	pipeline_config.p_renderpass = &context_->get_main_render_pass();
	pipeline_config.p_attributes = &binding_description;
	pipeline_config.p_descriptor_set_layouts = nullptr;
	pipeline_config.p_stages = &stage_create_infos;
	pipeline_config.viewport = viewport;
	pipeline_config.scissor = scissor;
	pipeline_config.vertex_stride = sizeof(SimpleShader::Vertex);

	// Create the pipeline
	auto created_pipeline =
			Pipeline::create_pipeline(
					&context_->get_device(),
					pipeline_config,
					false);

	// Check that it was created
	if (!created_pipeline.has_value())
	{
		throw std::runtime_error("Failed to create pipeline");
	}

	pipeline_ = std::move(created_pipeline.value());
}

void SimpleShader::use()
{
	pipeline_.bind(context_->get_command_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
}
}// namespace flwfrg::vk::shader