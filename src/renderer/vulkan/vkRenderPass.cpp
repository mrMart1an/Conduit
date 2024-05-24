#include "renderer/vulkan/vkCommandBuffer.h"
#include "renderer/vulkan/vkRenderAttachment.h"

#include "renderer/vulkan/vkRenderPass.h"

namespace cndt::vulkan {

// Begin the render pass on the given attachment
void RenderPass::begin(
    RenderAttachment &attachment,
    RenderArea render_area,
    
    CommandBuffer cmd_buffer
) {
    VkRenderPassBeginInfo rendering_info = { };
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rendering_info.renderPass = m_handle;
    rendering_info.framebuffer = attachment.frame_buffer();
    
    rendering_info.renderArea.offset.x = render_area.x;
    rendering_info.renderArea.offset.y = render_area.y;
    rendering_info.renderArea.extent.width = render_area.width;
    rendering_info.renderArea.extent.height = render_area.height;

    VkClearValue clear_color = { };
    clear_color.color.float32[0] = m_clear_color.r;
    clear_color.color.float32[1] = m_clear_color.g;
    clear_color.color.float32[2] = m_clear_color.b;
    clear_color.color.float32[3] = m_clear_color.a;

    rendering_info.clearValueCount = 1;
    rendering_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(
        cmd_buffer.handle(),
        &rendering_info,
        VK_SUBPASS_CONTENTS_INLINE
    );
}

// End the render pass
void RenderPass::end(CommandBuffer cmd_buffer)
{
    vkCmdEndRenderPass(cmd_buffer.handle());
}

} // namespace cndt::vulkan
