#include "conduit/logging.h"
#include "renderer/vulkan/utils/vkExceptions.h"

#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"
#include <sstream>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "renderer/vulkan/vkRenderPass.h"

namespace cndt::vulkan {

// Begin the render pass on the given attachment
void RenderPass::begin(
    CommandBuffer cmd_buffer,
    RenderArea render_area,

    std::vector<const VulkanImage*> attachments,
    std::vector<VkClearValue> clear_value
) {
    FrameBuffer& frame_buffer = getFrameBuffer(attachments);

    // Check render area compatibility
    if (render_area.x + render_area.width > frame_buffer.width) {
        throw RenderPassBeginError(
            "Incompatible render area width (x: {}, width: {})",
            render_area.x, render_area.width
        );
    }
    if (render_area.y + render_area.height > frame_buffer.height) {
        throw RenderPassBeginError(
            "Incompatible render area height (x: {}, height: {})",
            render_area.y, render_area.height
        );
    }

    VkRenderPassBeginInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

    render_pass_info.renderPass = m_handle;
    render_pass_info.framebuffer = frame_buffer.m_handle;

    render_pass_info.renderArea.offset.x = render_area.x;
    render_pass_info.renderArea.offset.y = render_area.y;
    render_pass_info.renderArea.extent.width = render_area.width;
    render_pass_info.renderArea.extent.height = render_area.height;

    render_pass_info.clearValueCount = clear_value.size();
    render_pass_info.pClearValues = clear_value.data();

    // Begin the render pass
    vkCmdBeginRenderPass(
        cmd_buffer.handle(),
        &render_pass_info,
        VK_SUBPASS_CONTENTS_INLINE
    );
}

// End the render pass
void RenderPass::end(CommandBuffer cmd_buffer)
{
    vkCmdEndRenderPass(cmd_buffer.handle());
}

// Get a frame buffer from the frame buffer cache or create a new one
// if the frame buffer is not cache
RenderPass::FrameBuffer& RenderPass::getFrameBuffer(
    const std::vector<const VulkanImage*>& attachments
) {
    if (attachments.size() != m_attachment_infos.size()) {
        throw IncompatibleAttachmentsError(
            "Incompatible number of attachments for render pass"
        );
    }

    for (int i = 0; i < m_attachment_infos.size(); i++) {
        m_keys_tmp[i] = attachments[i]->id();
    }

    // Get cached frame buffer from the cache or create a new one
    if (m_frame_buffer_cache.find(m_keys_tmp) != m_frame_buffer_cache.end()) {
        FrameBuffer& frame_buffer = m_frame_buffer_cache[m_keys_tmp];

        // Reset the unused counter
        frame_buffer.m_unused_counter = 0;

        return frame_buffer;
    } else {
        return createFrameBuffer(attachments);
    }
}

// Create a frame buffer and store it in the frame buffer cache
RenderPass::FrameBuffer& RenderPass::createFrameBuffer(
    const std::vector<const VulkanImage*>& attachments
) {
    log::core::trace("Allocating new frame buffer");

    if (attachments.size() != m_attachment_infos.size()) {
        throw IncompatibleAttachmentsError(
            "Incompatible number of attachments for render pass"
        );
    }

    // Check for attachments compatibility and determine buffer size
    u32 width = 0; 
    u32 height = 0;

    for (int i = 0; i < m_attachment_infos.size(); i++) {
        if (m_attachment_infos[i].format != attachments[i]->format()) {
            throw IncompatibleAttachmentsError(
                "Incompatible attachments format (attachment {})", i
            );
        }
        if (m_attachment_infos[i].samples != attachments[i]->sample()) {
            throw IncompatibleAttachmentsError(
                "Incompatible attachments sample count (attachment {})", i
            );
        }

        // Set width and height
        if (width == 0 || width == attachments[i]->extent().width) {
            width = attachments[i]->extent().width;
        } else {
            throw IncompatibleAttachmentsError(
                "Incompatible attachments width (attachment {}, width: {})", 
                i, attachments[i]->extent().width
            );
        }
        if (height == 0 || height == attachments[i]->extent().height) {
            height = attachments[i]->extent().height;
        } else {
            throw IncompatibleAttachmentsError(
                "Incompatible attachments height (attachment {}, height: {})", 
                i, attachments[i]->extent().height
            );
        }
    }

    // Create the frame buffer ids keys
    for (int i = 0; i < m_attachment_infos.size(); i++) {
        m_keys_tmp[i] = attachments[i]->id();
    }

    // Get the frame buffer from the cache
    FrameBuffer& frame_buffer = m_frame_buffer_cache[m_keys_tmp];

    // Generate attachments image views
    for (int i = 0; i < m_attachment_infos.size(); i++) {
        m_view_tmp[i] = attachments[i]->view();
    }

    VkFramebufferCreateInfo frame_buffer_info = { };
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    
    frame_buffer_info.renderPass = m_handle; 
    frame_buffer_info.width = width;
    frame_buffer_info.height = height;

    frame_buffer_info.attachmentCount = attachments.size();
    frame_buffer_info.pAttachments = m_view_tmp.data();
    
    frame_buffer_info.layers = 1;

    VkResult res = vkCreateFramebuffer(
        m_device_p->logical(),
        &frame_buffer_info,
        m_device_p->allocator(),
        &frame_buffer.m_handle
    ); 
    
    if (res != VK_SUCCESS) {
        throw RenderAttachmentCreationError(
            "frame buffer creation error {}",
            vk_error_str(res)
        );
    }

    // Store frame buffer info
    frame_buffer.width = width;
    frame_buffer.height = height;

    frame_buffer.m_unused_counter = 0;

    // Print the image ids for the frame buffer
    std::stringstream s;
    for (int i = 0; i < m_keys_tmp.size(); i++) {
        s << m_keys_tmp[i];

        if (i != m_keys_tmp.size()-1) {
            s << ", ";
        }
    }

    log::core::trace("Frame buffer key: [{}]", s.str());

    return frame_buffer;
}

// Delete a frame buffer
void RenderPass::deleteFrameBuffer(FrameBuffer& frame_buffer)
{
    log::core::trace("Deallocating old frame buffer");

    vkDestroyFramebuffer(
        m_device_p->logical(),
        frame_buffer.m_handle,
        m_device_p->allocator()
    );

    frame_buffer = { };
}

// Increment the delete counter and delete the unused frame buffer
void RenderPass::cleanupRoutine()
{
    for (
        auto iter = m_frame_buffer_cache.begin();
        iter != m_frame_buffer_cache.end();
    ) {
        FrameBuffer& frame_buffer = iter->second;
        frame_buffer.m_unused_counter += 1;

        if (frame_buffer.m_unused_counter > frame_buffer_delete_after) {
            deleteFrameBuffer(frame_buffer);

            iter = m_frame_buffer_cache.erase(iter);
        } else {
            ++iter;
        }
    }
}

} // namespace cndt::vulkan
