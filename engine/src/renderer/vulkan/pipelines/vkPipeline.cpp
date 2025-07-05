#include "renderer/vulkan/pipelines/vkPipeline.h"
#include "renderer/vulkan/vkCommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Bind the pipeline for use
void GraphicsPipeline::bind(
    CommandBuffer &cmd_buffer
) {
	vkCmdBindPipeline(
	    cmd_buffer.handle(),
	    VK_PIPELINE_BIND_POINT_GRAPHICS,
	    m_handle
	);
}

} // namespace cndt::vulkan
