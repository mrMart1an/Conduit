#ifndef CNDT_VK_SHADER_MODULE_H
#define CNDT_VK_SHADER_MODULE_H

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan shader module
class VulkanShaderModule {
    friend class Device;

public:
    VulkanShaderModule() = default;
    ~VulkanShaderModule() = default; 

    VkShaderModule handle() const { return m_handle; }

    // Return the shader stage info
    VkPipelineShaderStageCreateInfo stageInfo() const 
    {
        return m_stage_create_info;
    }

private:
    VkShaderModule m_handle;
    
    VkPipelineShaderStageCreateInfo m_stage_create_info;
};

} // namespace cndt::vulkan

#endif
