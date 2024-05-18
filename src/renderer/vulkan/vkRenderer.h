#ifndef CNDT_VK_RENDERER_H
#define CNDT_VK_RENDERER_H

#include "conduit/window/window.h"
#include "conduit/renderer/renderer.h"

#include "renderer/vulkan/initialization/vkContext.h"
#include "renderer/vulkan/vkDevice.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class VkRenderer : public Renderer {
public:
    VkRenderer() = default; 
    ~VkRenderer() override = default;

protected:
    // Initialize the renderer implementation
    void initialize(
        const char *app_title,
        Window *window_p
    ) override;
    
    // Shutdown the renderer implementation
    void shutdown() override;

    // Resize the renderer viewport
    void resize(u32 width, u32 height) override;

private:
    // Renderer delete queue
    DeleteQueue m_delete_queue;
    
    // Store the vulkan instance, surface, allocator and debug messenger 
    Context m_context;

    // Store logical, physical device and queue
    Device m_device;
};

} // namespace cndt::vulkan

#endif
