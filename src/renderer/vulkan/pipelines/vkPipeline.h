#ifndef CNDT_VK_PIPELINE_H
#define CNDT_VK_PIPELINE_H

#include "renderer/vulkan/pipelines/vkShaderModule.h"
#include "renderer/vulkan/vkCommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan pipeline class
class Pipeline {

friend class Device;

public:
    Pipeline() = default;
    
protected:
    VkPipeline m_handle;
    VkPipelineLayout m_layout;
};

// Vulkan graphic pipeline
class GraphicsPipeline : public Pipeline {

friend class Device;
    
public:
    // Bind the pipeline for use
    // Use the graphic bind port by default for this pipeline
    void bind(CommandBuffer &cmd_buffer);
    
private:
    ShaderModule m_vertex_stage;
    ShaderModule m_fragment_stage;
};

} // namespace cndt::vulkan

#endif
