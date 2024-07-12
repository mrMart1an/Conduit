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

    Type type
) :
    m_device_p(device_p),
    m_modules(std::move(modules)),

    m_rasterizer_info(rasterizer_info),
    m_multisampling_info(multisampling_info),
    m_depth_stencil_info(depth_stencil_info),

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
