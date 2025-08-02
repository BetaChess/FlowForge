#include "pch.hpp"

#include "model_manager.hpp"

#include "vulkan/device.hpp"

namespace flwfrg::vk
{
ColorModelManager::ColorModelManager(Device *device) : device_(device) {}

void ColorModelManager::reserve_vertex_buffer_space(uint64_t space_to_reserve)
{
    assert(device_ != nullptr);

    if (space_to_reserve == 0)
    {
        return;
    }
    if (space_to_reserve < remaining_vertex_buffer_space_)
    {
        return;
    }

    vertex_buffers_.emplace_back(device_, space_to_reserve,
                                 static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
    current_vertex_buffer_index_++;
    remaining_vertex_buffer_space_ = space_to_reserve;
}

void ColorModelManager::reserve_index_buffer_space(uint64_t space_to_reserve)
{
    assert(device_ != nullptr);

    if (space_to_reserve == 0)
    {
        return;
    }
    if (space_to_reserve < remaining_index_buffer_space_)
    {
        return;
    }

    index_buffers_.emplace_back(device_, space_to_reserve,
                                static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, false);
    current_index_buffer_index_++;
    remaining_index_buffer_space_ = space_to_reserve;
}

ColorModelManager::object_id_t ColorModelManager::register_model(std::vector<ColorVertex> vertices)
{
    // TODO: Implement
    throw std::runtime_error("ColorModelManager::register_model with vertices only is not implemented yet. "
                           "Please provide indices as well.");
}

ColorModelManager::object_id_t ColorModelManager::register_model(std::vector<ColorVertex> vertices,
                                                                 std::vector<uint32_t> indices)
{
    assert(device_ != nullptr);

    uint64_t vertex_size = sizeof(ColorVertex) * vertices.size();
    uint64_t index_size = sizeof(uint32_t) * indices.size();

    reserve_vertex_buffer_space(vertex_size);
    reserve_index_buffer_space(index_size);

    uint64_t vertex_offset =
            vertex_buffers_[current_vertex_buffer_index_].get_total_size() - remaining_vertex_buffer_space_;
    uint64_t index_offset =
            index_buffers_[current_index_buffer_index_].get_total_size() - remaining_index_buffer_space_;

    vertex_buffers_[current_vertex_buffer_index_].upload_data(vertices.data(), vertex_offset, vertex_size,
                                                              device_->get_graphics_command_pool(), nullptr,
                                                              device_->get_graphics_queue());
    index_buffers_[current_index_buffer_index_].upload_data(indices.data(), index_offset, index_size,
                                                            device_->get_graphics_command_pool(), nullptr,
                                                            device_->get_graphics_queue());

    object_data_list_.emplace_back(ObjectData{
            .vertex_buffer_index = static_cast<int32_t>(current_vertex_buffer_index_),
            .index_buffer_index = static_cast<int32_t>(current_index_buffer_index_),
            .vertex_offset = vertex_offset,
            .index_offset = index_offset,
            .index_count = static_cast<uint32_t>(vertices.size()),
    });
    object_transforms_.emplace_back(Transform{});
    cached_mat4s_.emplace_back(object_transforms_.back().mat4());

    remaining_index_buffer_space_ -= index_size;
    remaining_vertex_buffer_space_ -= vertex_size;

    return static_cast<object_id_t>(object_data_list_.size() - 1);
}

// Currently a noop
void ColorModelManager::unregister_model(object_id_t id) {}

} // namespace flwfrg::vk
