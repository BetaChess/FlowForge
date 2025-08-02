#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <filesystem>
#include <glm/glm.hpp>

#include "math/transform.hpp"
#include "vulkan/buffer.hpp"
#include "vulkan/shader/vertex.hpp"

namespace flwfrg::vk
{

class ColorModelManager
{
public:
    typedef int32_t object_id_t;

    struct GeometryRenderData
    {
        glm::mat4 model{1.0f};
    };

    struct ModelRenderInfo
    {
        VkBuffer vertex_buffer;
        VkBuffer index_buffer;
        uint64_t vertex_offset = 0;
        uint64_t index_offset = 0;
        uint32_t index_count = 0;
        GeometryRenderData render_data{};
    };

public:
    ColorModelManager() = default;
    ColorModelManager(Device *device);

    // Copy
    ColorModelManager(const ColorModelManager &) = delete;
    ColorModelManager &operator=(const ColorModelManager &) = delete;

    // Move
    ColorModelManager(ColorModelManager &&) = default;
    ColorModelManager &operator=(ColorModelManager &&) = default;

    // Methods

    void reserve_vertex_buffer_space(uint64_t space_to_reserve);
    void reserve_index_buffer_space(uint64_t space_to_reserve);

    object_id_t register_model(std::vector<ColorVertex> vertices);
    object_id_t register_model(std::vector<ColorVertex> vertices, std::vector<uint32_t> indices);

    void unregister_model(object_id_t id);

    // Model access

    inline const Transform &get_transform(object_id_t id) const
    {
        assert(id >= 0 && id < static_cast<object_id_t>(object_transforms_.size()));
        return object_transforms_[id];
    }

    inline void set_transform(object_id_t id, const Transform &transform)
    {
        assert(id >= 0 && id < static_cast<object_id_t>(object_transforms_.size()));
        object_transforms_[id] = transform;
        cached_mat4s_[id] = transform.mat4();
    }

    inline ModelRenderInfo get_model_render_info(object_id_t id) const
    {
        assert(id >= 0 && id < static_cast<object_id_t>(object_data_list_.size()));
        const ObjectData &data = object_data_list_[id];
        return ModelRenderInfo{
            .vertex_buffer = vertex_buffers_[data.vertex_buffer_index].get_handle(),
            .index_buffer = index_buffers_[data.index_buffer_index].get_handle(),
            .vertex_offset = data.vertex_offset,
            .index_offset = data.index_offset,
            .index_count = data.index_count,
            .render_data = GeometryRenderData{.model = cached_mat4s_[id]},
        };
    }

private:
    struct ObjectData
    {
        int32_t vertex_buffer_index = -1;
        int32_t index_buffer_index = -1;
        uint64_t vertex_offset = 0;
        uint64_t index_offset = 0;
        uint32_t index_count = 0;
    };

    Device *device_ = nullptr;

    // Uses object id as index
    std::vector<ObjectData> object_data_list_{};
    std::vector<Transform> object_transforms_{};
    std::vector<glm::mat4> cached_mat4s_{};

    // Index stored in ObjectData
    std::vector<Buffer> vertex_buffers_{};
    std::vector<Buffer> index_buffers_{};

    // TODO: Make free lists for buffers and make unregister not be a noop

    // Simple growing only buffers

    uint64_t current_vertex_buffer_index_ = std::numeric_limits<uint64_t>::max();
    uint64_t current_index_buffer_index_ = std::numeric_limits<uint64_t>::max();
    uint64_t remaining_vertex_buffer_space_ = 0;
    uint64_t remaining_index_buffer_space_ = 0;
};

} // namespace flwfrg::vk
