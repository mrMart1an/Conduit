#ifndef CNDT_VK_RENDER_PASS_H
#define CNDT_VK_RENDER_PASS_H

#include "conduit/defines.h"

#include "conduit/renderer/image.h"
#include "renderer/vulkan/storage/vkImage.h"
#include "renderer/vulkan/vkCommandBuffer.h"
#include "renderer/vulkan/vkRenderAttachment.h"

#include <glm/ext/vector_float4.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Conduit vulkan render pass abstraction
class RenderPass {
    friend class Device;

public:
    // Render pass render area
    struct RenderArea {
        RenderArea(u32 width, u32 height) :
            x(0), y(0), width(width), height(height) { };
        RenderArea(i32 x, i32 y, u32 width, u32 height) :
            x(x), y(y), width(width), height(height) { };

        i32 x, y;  
        u32 width, height;
    };

    // Render pass dependency struct 
    struct Dependency {
        VkPipelineStageFlags src_stage_mask = 0;
        VkAccessFlags src_access_mask = 0;

        VkPipelineStageFlags dst_stage_mask = 0;
        VkAccessFlags dst_access_mask = 0;

        bool by_region = true;
    };

    // Render pass attachment descriptor
    struct Attachment {
        // Attachment format
        GpuImage::Info::Format format;
        // Attachment samples count
        GpuImage::Info::Sample samples;
        
        // Color and depth load and store operation
        VkAttachmentLoadOp load_op;
        VkAttachmentStoreOp store_op;
        
        // Stencil load and store operation
        VkAttachmentLoadOp stencil_load_op;
        VkAttachmentStoreOp stencil_store_op;
        
        // Layout of the image before the pass begin
        VkImageLayout initial_layout;
        // Layout of the image required at the end of the pass
        VkImageLayout final_layout;
    };

    // struct subpass description
    struct Subpass {
        std::vector<VkAttachmentReference> color_attachments;
        std::vector<VkAttachmentReference> input_attachments;
        std::vector<VkAttachmentReference> resolve_attachments;

        std::optional<VkAttachmentReference> depth_stencil_attachment;
    };

    // Frame buffer storage
    struct FrameBuffer {
        VkFramebuffer m_handle;

        // Frame buffer size
        u32 width, height;

        // Count unused counter to mark frame buffer for delete
        u32 m_unused_counter;
    };

    // Number of frame after which an unused frame buffer need to be deleted 
    static constexpr u32 frame_buffer_delete_after = 10;

public:
    RenderPass() = default;
    
    // Begin the render pass on the given attachment.
    //
    // The clear value vector is index by attachment number even if 
    // the attachment use load op load or ignore
    void begin(
        CommandBuffer cmd_buffer,
        RenderArea render_area,

        std::vector<const VulkanImage*> attachments,
        std::vector<VkClearValue> clear_value
    );

    // End the render pass
    void end(CommandBuffer cmd_buffer);

    // Render pass memory cleanup routine to run every frame
    void cleanupRoutine();

private:
    // Get a frame buffer from the frame buffer cache or create a new one
    // if the frame buffer is not cache
    FrameBuffer& getFrameBuffer(
        const std::vector<const VulkanImage*>& attachments
    );

    // Create a frame buffer and store it in the frame buffer cache
    FrameBuffer& createFrameBuffer(
        const std::vector<const VulkanImage*>& attachments
    );

    // Delete a frame buffer
    void deleteFrameBuffer(FrameBuffer& frame_buffer);

private:
    VkRenderPass m_handle;

    // Store the attachments infos information
    std::vector<Attachment> m_attachment_infos;

    // Frame buffer cache 
    std::map<std::vector<u32>, FrameBuffer> m_frame_buffer_cache;

    // Pointer to the device that own the render pass
    Device* m_device_p;

    // Temporary keys and views buffer to avoid frequent heap allocation 
    std::vector<u32> m_keys_tmp;
    std::vector<VkImageView> m_view_tmp;
};

} // namespace cndt::vulkan

#endif
