#ifndef CNDT_VK_RENDERER_H
#define CNDT_VK_RENDERER_H

#include "conduit/assets/assetsManager.h"
#include "conduit/config/engineConfig.h"
#include "conduit/renderer/vertex.h"
#include "conduit/window/window.h"
#include "conduit/renderer/renderer.h"

#include "renderer/vulkan/descriptor/vkDescriptorAllocator.h"
#include "renderer/vulkan/descriptor/vkDescriptorLayout.h"
#include "renderer/vulkan/pipelines/vkPipeline.h"
#include "renderer/vulkan/storage/vkGeometryBuffer.h"
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
    struct InFlightData {
        CommandPool graphics_cmd_pool;
        CommandBuffer main_cmd_buffer;

        // Sync objects
        Fence render_fence;
        VkSemaphore image_semaphore, render_semaphore;

        // Uniform buffer
        VulkanBuffer camera_model_uniform;

        // descriptor allocator and writer
        DescriptorAllocator descriptor_allocator;
        DescriptorWriter descriptor_writer; // This doesn't need to be here
    };

public:
    // Draw a frame and present it
    void draw() override;
    
    // Set renderer v-sync 
    void setVsync(bool v_sync) override;

    // Toggle renderer v-sync
    void toggleVsync() override;

    // Get a shader program builder
    RendererResRef<ShaderProgramBuilder> getShaderProgramBuilder() override;

protected:
    // Initialize the renderer implementation
    void initialize(
        EngineConfig::Renderer config,
        const char *app_title,

        Window *window_p
    ) override;
    
    // Shutdown the renderer implementation
    void shutdown() override;

    // Resize the renderer viewport
    void resize(u32 width, u32 height) override;

private:

    // Begin frame rendering, return true if the frame was begun successfully 
    // and the rendering can proceed
    bool beginFrame();

    // End frame rendering
    void endFrame();

    // Present the current swap chain frame, return true if the frame was 
    // presented successfully to the screen
    bool presentFrame();

    // Recreate the swap chain and the swap chain render attachments
    void recreateSwapChain();

    /*
     *
     *      Frame data functions
     *
     * */

    // Create and initialized all the frame in flight data
    void createInFlightDatas();
    
    // Destroy all the frame in flight data
    void destroyInFlightData();

    // Get the current frame in flight data
    InFlightData& getCurrentInFlightData();

    /*
     *
     *      Render attachment functions
     *
     * */

    // Create the swap chain render attachments using the main render pass
    void createSwapChainAttachment();

    // Destroy the swap chain render attachments
    void destroySwapChainAttachment();

    // Get the current swap chain render attachment
    RenderAttachment& getCurrentAttachment();

private:
    // Renderer delete queue
    DeleteQueue m_delete_queue;
    
    // Store the vulkan instance, surface, allocator and debug messenger 
    Context m_context;

    // Store logical, physical device and queue
    Device m_device;

    SwapChain m_swap_chain;

    GraphicsPipeline m_graphics_pipeline;

    GeometryBuffer<Vertex3D> m_static_mesh_buffer;

    DescriptorLayout m_uniform_layout;

    // Render pass and related swap chain render attachment
    RenderPass m_main_render_pass;
    std::vector<RenderAttachment> m_swap_chain_attachements;

    // Frame in flight data 
    std::vector<InFlightData> m_in_flight_data;

    // Dimensions of the renderer swap chain attachments
    u32 m_frame_width, m_frame_height;

    // Store true and halt rendering if the window is minimized
    bool m_minimized;

    // Count the total number of frame rendered by the renderer
    u64 m_frame_count;

    // TODO temporary asset manager, remove later
    AssetsManager m_asset_manager;
};

} // namespace cndt::vulkan

#endif
