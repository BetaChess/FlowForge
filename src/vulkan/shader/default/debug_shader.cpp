#include "pch.hpp"

#include "debug_shader.hpp"

#include "vulkan/display_context.hpp"
#include "vulkan/shader/vertex.hpp"

namespace flwfrg::vk::shader
{

DebugShader::DebugShader(DisplayContext *context) : context_{context}
{
    assert(context_ != nullptr);

    VkShaderStageFlagBits stage_types[shader_stage_count] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

    // Iterate over each stage
    for (auto &stage_type : stage_types)
    {
        // Create a shader stage
        auto stage = ShaderStage::create_shader_module(&context_->get_device(), shader_file_name, stage_type);

        if (!stage.has_value())
        {
            throw std::runtime_error("Failed to create shader stage");
        }

        stages_.emplace_back(std::move(stage.value()));
    }

    // Descriptors
    // Global descriptors
    VkDescriptorSetLayoutBinding global_ubo_layout_binding{};
    global_ubo_layout_binding.binding = 0;
    global_ubo_layout_binding.descriptorCount = 1;
    global_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_ubo_layout_binding.pImmutableSamplers = nullptr;
    global_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo global_layout_info{};
    global_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    global_layout_info.bindingCount = 1;
    global_layout_info.pBindings = &global_ubo_layout_binding;
    global_descriptor_set_layout_ = DescriptorSetLayout(&context_->get_device(), global_layout_info);

    // Global descriptor pool
    VkDescriptorPoolSize global_pool_size{};
    global_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    global_pool_size.descriptorCount = context_->get_swapchain().get_image_count();

    VkDescriptorPoolCreateInfo global_pool_info{};
    global_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    global_pool_info.poolSizeCount = 1;
    global_pool_info.pPoolSizes = &global_pool_size;
    global_pool_info.maxSets = context_->get_swapchain().get_image_count();

    global_descriptor_pool_ = DescriptorPool(&context_->get_device(), global_pool_info);

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
    auto binding_description = ColorVertex::get_binding_description();

    // Descriptor set layouts
    // TODO: Change this to not be vector
    std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
            global_descriptor_set_layout_.handle(),
    };


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
    pipeline_config.p_descriptor_set_layouts = &descriptor_set_layouts; // TODO: also needs changing here
    pipeline_config.p_stages = &stage_create_infos;
    pipeline_config.viewport = viewport;
    pipeline_config.scissor = scissor;
    pipeline_config.vertex_stride = sizeof(ColorVertex);

    // Create the pipelines

    // Wireframe
    auto created_pipeline = Pipeline::create_pipeline(&context_->get_device(), pipeline_config, true);

    // Check that it was created
    if (!created_pipeline.has_value())
    {
        throw std::runtime_error("Failed to create pipeline");
    }

    pipeline_wire_frame_ = std::move(created_pipeline.value());

    // Solid
    created_pipeline = Pipeline::create_pipeline(&context_->get_device(), pipeline_config, false);

    // Check that it was created
    if (!created_pipeline.has_value())
    {
        throw std::runtime_error("Failed to create solid pipeline");
    }

    pipeline_solid_ = std::move(created_pipeline.value());

    // Create global uniform buffer
    global_uniform_buffer_ = Buffer(
            &context_->get_device(), sizeof(GlobalUniformObject) * 3,
            static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT),
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            true);

    // Allocate global descriptor sets
    std::vector<VkDescriptorSetLayout> global_layouts = {global_descriptor_set_layout_.handle(),
                                                         global_descriptor_set_layout_.handle(),
                                                         global_descriptor_set_layout_.handle()};

    VkDescriptorSetAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocate_info.descriptorPool = global_descriptor_pool_.handle();
    allocate_info.descriptorSetCount = global_layouts.size();
    allocate_info.pSetLayouts = global_layouts.data();
    // Allocate it
    if (vkAllocateDescriptorSets(context_->get_device().get_logical_device(), &allocate_info,
                                 global_descriptor_sets_.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate descriptor sets");
    }
}

void DebugShader::update_global_state(glm::mat4 projection, glm::mat4 view)
{
    CommandBuffer &command_buffer = context_->get_command_buffer();
    auto current_frame = context_->get_current_frame();
    //auto image_index = context_->get_image_index();
    VkDescriptorSet global_descriptor = global_descriptor_sets_[current_frame];

    use();

    global_ubo.projection = projection;
    global_ubo.view = view;

    // Configure the descriptors for the given index
    uint32_t range = sizeof(GlobalUniformObject);
    uint64_t offset = sizeof(GlobalUniformObject) * current_frame;

    // Copy data to buffer
    global_uniform_buffer_.load_data(&global_ubo, offset, range, 0);

    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = global_uniform_buffer_.get_handle();
    buffer_info.offset = offset;
    buffer_info.range = range;

    // Update the descriptor sets

    // Global ubo
    VkWriteDescriptorSet ubo_descriptor_write{};
    ubo_descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    ubo_descriptor_write.dstSet = global_descriptor;
    ubo_descriptor_write.dstBinding = 0;
    ubo_descriptor_write.dstArrayElement = 0;
    ubo_descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo_descriptor_write.descriptorCount = 1;
    ubo_descriptor_write.pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(context_->get_device().get_logical_device(), 1, &ubo_descriptor_write, 0, nullptr);
    global_descriptor_updated_[current_frame] = true;

    // Bind descriptor set
    if (using_wire_frame_)
    {
        vkCmdBindDescriptorSets(command_buffer.get_handle(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                pipeline_wire_frame_.layout(), 0, 1, &global_descriptor, 0, nullptr);
    } else
    {
        vkCmdBindDescriptorSets(command_buffer.get_handle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_solid_.layout(),
                                0, 1, &global_descriptor, 0, nullptr);
    }
}

void DebugShader::update_object(ColorModelManager::GeometryRenderData data)
{
    CommandBuffer &command_buffer = context_->get_command_buffer();
    if (using_wire_frame_)
    {
        vkCmdPushConstants(command_buffer.get_handle(), pipeline_wire_frame_.layout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(glm::mat4), &data.model);
    } else
    {
        vkCmdPushConstants(command_buffer.get_handle(), pipeline_solid_.layout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(glm::mat4), &data.model);
    }
}

void DebugShader::use()
{
    if (using_wire_frame_)
    {
        pipeline_wire_frame_.bind(context_->get_command_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
    }
    else
    {
        pipeline_solid_.bind(context_->get_command_buffer(), VK_PIPELINE_BIND_POINT_GRAPHICS);
    }
}

} // namespace flwfrg::vk::shader
