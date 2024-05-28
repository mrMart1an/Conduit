#ifndef CNDT_VK_SHADER_MODULE_H
#define CNDT_VK_SHADER_MODULE_H

#include <vulkan/vulkan_core.h>
namespace cndt::vulkan {

class Device;

// Vulkan shader module
class ShaderModule {
    friend class Device;

public:
    ShaderModule() = default;

private:
    VkShaderModule m_handle;
    
    VkShaderModuleCreateInfo m_create_info;
    VkPipelineShaderStageCreateInfo m_shader_stage_create_info;
};

} // namespace cndt::vulkan

#endif
