#ifndef CNDT_VK_RENDER_PASS_H
#define CNDT_VK_RENDER_PASS_H

#include "conduit/defines.h"

#include "renderer/vulkan/vkCommandBuffer.h"
#include "renderer/vulkan/vkRenderAttachment.h"

#include <glm/ext/vector_float4.hpp>
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Conduit vulkan render pass abstraction
class RenderPass {
    friend class Device;

public:
    struct RenderArea {
        RenderArea(u32 width, u32 height) :
            x(0), y(0), width(width), height(height) { };
        RenderArea(u32 width, u32 height, i32 x, i32 y) :
            x(x), y(y), width(width), height(height) { };

        i32 x, y;  
        u32 width, height;
    };

    struct ClearColor {
        ClearColor(glm::vec4 color) : 
            r(color.r), g(color.g), b(color.b), a(color.a) { };
        ClearColor(f32 r, f32 g, f32 b, f32 a) : 
            r(r), g(g), b(b), a(a) { };
        ClearColor() : 
            r(0), g(0), b(0), a(0) { };
        
        f32 r, g, b, a;
    };

public:
    // Begin the render pass on the given attachment
    void begin(
        RenderAttachment &attachment,
        RenderArea render_area,
        
        CommandBuffer cmd_buffer
    );

    // End the render pass
    void end(CommandBuffer cmd_buffer);

private:
    // Private constructor, only the device class can create render passes
    RenderPass(ClearColor clear_color) :
        m_handle(VK_NULL_HANDLE), m_clear_color(clear_color) { };

private:    
    VkRenderPass m_handle;
    ClearColor m_clear_color;
};

} // namespace cndt::vulkan

#endif
