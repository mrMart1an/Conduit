#ifndef CNDT_VK_RENDERER_H
#define CNDT_VK_RENDERER_H

#include "conduit/window/window.h"
#include "conduit/renderer/renderer.h"

#include "renderer/vulkan/vkContext.h"
#include "renderer/vulkan/vkDevice.h"
#include "renderer/vulkan/vkRenderAttachment.h"
#include "renderer/vulkan/vkRenderPass.h"
#include "renderer/vulkan/vkSwapChain.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class VkRenderer : public Renderer {
public:
    VkRenderer() = default; 
    ~VkRenderer() override = default;

public:
    // Frame in flight data
    struct FrameData {
        CommandPool graphics_cmd_pool;
        CommandBuffer main_cmd_buffer;

        // Sync objects
        VkFence render_fence;
        VkSemaphore image_semaphore, render_semaphore;
    };

public:
    // Draw a frame and present it
    void draw() override;

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

    // Recreate the swap chain and the swap chain render attachments
    void recreateSwapChain(u32 width, u32 height);

    /*
     *
     *      Frame data functions
     *
     * */

    // Create and initialized all the frame in flight data
    void createFrameDatas();
    
    // Destroy all the frame in flight data
    void destroyFrameData();

    // Get the current frame in flight data
    FrameData& getCurrentFrame();

    /*
     *
     *      Render attachment functions
     *
     * */

    // Create the swap chain render attachments using the main render pass
    void createSwapChainAttachment();

    // Destroy the swap chain render attachments
    void destroySwapChainAttachment();

private:
    // Renderer delete queue
    DeleteQueue m_delete_queue;
    
    // Store the vulkan instance, surface, allocator and debug messenger 
    Context m_context;

    // Store logical, physical device and queue
    Device m_device;

    SwapChain m_swap_chain;

    // Render pass and related swap chain render attachment
    RenderPass m_main_render_pass;
    std::vector<RenderAttachment> m_swap_chain_attachements;

    // Frame in flight data 
    std::vector<FrameData> m_frames_data;

    // Store true and halt rendering if the window is minimized
    bool m_minimized;
};

} // namespace cndt::vulkan

#endif
