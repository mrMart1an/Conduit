#ifndef CNDT_VK_SHADER_PROGRAM_H
#define CNDT_VK_SHADER_PROGRAM_H

#include "conduit/defines.h"

#include "conduit/renderer/shader/program.h"
#include "renderer/vulkan/pipelines/vkShaderModule.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;
class VulkanShaderProgramBuilder;

// Vulkan shader program
class VulkanShaderProgram : public ShaderProgram {
    friend class VulkanShaderProgramBuilder;

public:
    VulkanShaderProgram(
        Device *device_p,
        std::vector<VulkanShaderModule> modules,

        VkPipelineRasterizationStateCreateInfo &rasterizer_info,
        VkPipelineMultisampleStateCreateInfo &multisampling_info,

        VkPipelineDepthStencilStateCreateInfo &depth_stencil_info,

        Type type
    );
    ~VulkanShaderProgram() override;

    // Return the renderer backend that own this resource
    RendererBackend backend() const override 
    { return RendererBackend::Vulkan; }

    // Return a pointer to the shader stage info, store the stage count 
    // in the given u32 pointer
    const VkPipelineShaderStageCreateInfo* getStageInfo(u32 *count_p) const
    {
        if (count_p != nullptr)
            *count_p = static_cast<u32>(m_stage_infos.size());

        return m_stage_infos.data();
    }

    // Return a pointer to the rasterizer info
    const VkPipelineRasterizationStateCreateInfo* getRasterizerInfo() const 
    { return &m_rasterizer_info; }

    // Return a pointer to the multi sampling info
    const VkPipelineMultisampleStateCreateInfo* getMultisamplingInfo() const 
    { return &m_multisampling_info; }

    // Return a pointer to the depth stencil sampling info
    const VkPipelineDepthStencilStateCreateInfo* getDepthStencilInfo() const 
    { return &m_depth_stencil_info; }

private:
    // Shader stage modules and info
    std::vector<VulkanShaderModule> m_modules;
    std::vector<VkPipelineShaderStageCreateInfo> m_stage_infos;

    // Rasterizer and multi sampling pipeline create info
    VkPipelineRasterizationStateCreateInfo m_rasterizer_info;
    VkPipelineMultisampleStateCreateInfo m_multisampling_info;

    // Depth stencil state info
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil_info;

    // Pointer to the device that own the program
    Device *m_device_p;
};

}

#endif
