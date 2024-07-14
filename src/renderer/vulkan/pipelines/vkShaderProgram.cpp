#include "conduit/renderer/shader/program.h"

#include "renderer/vulkan/pipelines/vkShaderProgram.h"
#include "renderer/vulkan/vkDevice.h"

#include <utility>

namespace cndt::vulkan {

VulkanShaderProgram::VulkanShaderProgram(
    Device *device_p,
    std::vector<VulkanShaderModule> modules,

    VkPipelineRasterizationStateCreateInfo &rasterizer_info,
    VkPipelineMultisampleStateCreateInfo &multisampling_info,

    VkPipelineDepthStencilStateCreateInfo &depth_stencil_info,
        
    VkVertexInputBindingDescription &vertex_binding_desc,
    std::vector<VkVertexInputAttributeDescription> vertex_attribute_desc,

    Type type
) :
    m_device_p(device_p),
    m_modules(std::move(modules)),

    m_rasterizer_info(rasterizer_info),
    m_multisampling_info(multisampling_info),
    m_depth_stencil_info(depth_stencil_info),

    m_vertex_binding_desc(vertex_binding_desc),
    m_vertex_attribute_desc(std::move(vertex_attribute_desc)),

    ShaderProgram(type)
{
    // Fill the shader stage info vector
    m_stage_infos.reserve(m_modules.size());

    for (auto& module : m_modules) {
        m_stage_infos.push_back(module.stageInfo());
    }
}

VulkanShaderProgram::~VulkanShaderProgram() 
{ 
    for (auto& module : m_modules) {
        m_device_p->destroyShaderModule(module);
    }
}

} // namespace cndt
