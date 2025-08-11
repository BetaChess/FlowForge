
#include <flowforge.hpp>
#include <vulkan/vulkan_context.hpp>

int main()
{
    flwfrg::init();

    flwfrg::vk::VulkanContext::CreateInfo create_info
    {
        .application_name = "Multiwindow Demo",
        .validation_layers = flwfrg::vk::VulkanContext::get_default_validation_layers(),
        .extensions = flwfrg::vk::VulkanContext::get_default_extensions(),
    };

    flwfrg::vk::VulkanContext vk_context;
    {
        auto result = flwfrg::vk::VulkanContext::create(create_info);
        if (!result.has_value())
        {
            FLOWFORGE_FATAL("Failed to create Vulkan Context (code: {})", static_cast<uint32_t>(result.error()));
            exit(1);
        }
        vk_context = std::move(result.value());
    }



    return 0;
}
