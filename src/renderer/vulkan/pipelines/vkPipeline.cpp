#include "renderer/vulkan/pipelines/vkPipeline.h"
#include "renderer/vulkan/vkCommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Bind the pipeline for use
void Pipeline::bind(
    CommandBuffer &cmd_buffer,
    VkPipelineBindPoint bind_port
) {
	vkCmdBindPipeline(cmd_buffer.handle(), bind_port, m_handle);
}

} // namespace cndt::vulkan
