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

        VkVertexInputBindingDescription &vertex_binding_desc,
        std::vector<VkVertexInputAttributeDescription> vertex_attribute_desc,

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

    // Return a pointer to vertex binding description
    const VkVertexInputBindingDescription* getVertexBindingDescription() const
    { return &m_vertex_binding_desc; }
 
    // Return a pointer to the vertex attribute description, 
    // store the attributes count in the given u32 pointer
    const VkVertexInputAttributeDescription* getVertexAttributeDescription(
        u32 *count_p
    ) const {
        if (count_p != nullptr)
            *count_p = static_cast<u32>(m_vertex_attribute_desc.size());

        return m_vertex_attribute_desc.data();
    }

private:
    // Shader stage modules and info
    std::vector<VulkanShaderModule> m_modules;
    std::vector<VkPipelineShaderStageCreateInfo> m_stage_infos;

    // Rasterizer and multi sampling pipeline create info
    VkPipelineRasterizationStateCreateInfo m_rasterizer_info;
    VkPipelineMultisampleStateCreateInfo m_multisampling_info;

    // Depth stencil state info
    VkPipelineDepthStencilStateCreateInfo m_depth_stencil_info;

    // Vertex binding and attribute description
    VkVertexInputBindingDescription m_vertex_binding_desc;
    std::vector<VkVertexInputAttributeDescription> m_vertex_attribute_desc;

    // Pointer to the device that own the program
    Device *m_device_p;
};

}

#endif
