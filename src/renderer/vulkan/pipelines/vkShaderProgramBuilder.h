#ifndef CNDT_VK_SHADER_PROGRAM_BUILDER_H
#define CNDT_VK_SHADER_PROGRAM_BUILDER_H

#include "renderer/vulkan/vkDevice.h"
#include "conduit/renderer/shader/programBuilder.h"

#include <vector>

#include "spirv_common.hpp"

namespace cndt::vulkan {

class VulkanShaderProgramBuilder : public ShaderProgramBuilder {
public:
    VulkanShaderProgramBuilder(
        Device *device_p
    ) : 
        m_device_p(device_p) 
    { }

    ~VulkanShaderProgramBuilder() override = default;

    // Build the shader program from the information 
    // and shader stage currently stored in the builder
    RenderRef<ShaderProgram> buildCache() override;

    // Return the renderer backend that own this resource
    RendererBackend backend() const override
    { return RendererBackend::Vulkan;}

    // Parse the rasterizer information 
    VkPipelineRasterizationStateCreateInfo parseRasterizationInfo() const;

    // Parse the multi sampling information 
    VkPipelineMultisampleStateCreateInfo parseMultisamplingInfo() const;

    // Parse depth stencil information
    VkPipelineDepthStencilStateCreateInfo parseDepthStencilInfo() const;

    // Parse vertex binding description
    VkVertexInputBindingDescription parseVertexBinding() const;

    // Parse vertex attribute description
    std::vector<VkVertexInputAttributeDescription> 
    parseVertexAttribute() const;

    // Parse the shader modules
    std::vector<VulkanShaderModule> parseShaderModules() const;

    // Convert vertex input and count to vkFormat
    VkFormat getVkFormat(
        VertexLayout::Format format,
        u32 size,
        spirv_cross::SPIRType loc_type
    ) const;

private:
    // Pointer to the device that own the builder
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
