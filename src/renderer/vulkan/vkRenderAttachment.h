#ifndef CNDT_VK_RENDER_ATTACHMENT_H
#define CNDT_VK_RENDER_ATTACHMENT_H

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Vulkan render attachment
class RenderAttachment {
    friend class Device;
    
public:
    RenderAttachment() = default;
    
    // Return the attachment format
    VkFormat format() const { return m_format; }

    // Return the attachment extent
    VkExtent2D extent() const { return m_extent; }

    // Return the frame buffer handle
    VkFramebuffer frame_buffer() const { return m_frame_buffer; }
    
private:
    VkFramebuffer m_frame_buffer;
    
    VkExtent2D m_extent;
    VkFormat m_format;
};

} // namespace cndt::vulkan

#endif
