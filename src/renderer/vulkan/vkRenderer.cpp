#include "renderer/vulkan/vkRenderer.h"
#include "renderer/vulkan/vkDevice.h"

#include <functional>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

/*
 *
 *      Initialization code
 *
 * */

// Initialize the renderer implementation
void VkRenderer::initialize(
    const char *app_title,
    Window *window_p
) {
    // Initialize the vulkan context
    m_context.initialize(app_title, window_p, VK_NULL_HANDLE);
    m_delete_queue.addDeleter(std::bind(&Context::shutdown, &m_context));

    // Initialize the vulkan device
    Device::PhysicalDeviceRequirement requirement = {};
    
    // Enable swap chain extensions
    requirement.required_device_extensions = Device::Extensions(true);
    // Require graphics, transfer and present of queue
    requirement.required_queue =
        Device::QueueFamilyType(true, true, true, true);
    // Require features 
    requirement.required_feature.geometryShader = VK_TRUE;
    requirement.required_feature.fillModeNonSolid = VK_TRUE;
    
    m_device.initialize(&m_context, requirement);
    m_delete_queue.addDeleter(std::bind(&Device::shutdown, &m_device));

    // Initialize the swap chain
    m_swap_chain.initialize(
        m_context,
        m_device,
        2,
        window_p->getWindowData().buffer_width,
        window_p->getWindowData().buffer_height,
        true 
    );
    m_delete_queue.addDeleter(std::bind(
        &SwapChain::shutdown,
        &m_swap_chain,
        std::ref(m_context),
        std::ref(m_device)
    ));

    // Create the main render pass
    m_main_render_pass = m_device.createRenderPass(
        m_swap_chain.format(),
        { 1., 0., 0., 1.}
    );
    m_delete_queue.addDeleter(std::bind(
        &Device::destroyRenderPass,
        &m_device,
        std::ref(m_main_render_pass)
    ));

    // Create the swap chain render attachments
    createSwapChainAttachment();
    m_delete_queue.addDeleter(std::bind(
        &VkRenderer::destroySwapChainAttachment,
        this
    ));
    
    // Create the frame in flight data
    createFrameDatas();
    m_delete_queue.addDeleter(std::bind(
        &VkRenderer::destroyFrameData,
        this
    ));

    // Create a graphics pipeline
    m_graphics_pipeline = m_device.createGraphicsPipeline(
        m_main_render_pass,
        "resources/shaders/builtin.vert.spv",
        "resources/shaders/builtin.frag.spv"
    );
    m_delete_queue.addDeleter(std::bind(
        &Device::destroyGraphicsPipeline,
        &m_device,
        std::ref(m_graphics_pipeline)
    ));

    // Create the static mesh geometry buffer
    m_static_mesh_buffer = m_device.createGeometryBuffer<Vertex3D>(128, 128);
    m_delete_queue.addDeleter(std::bind(
        &Device::destroyGeometryBuffer<Vertex3D>,
        &m_device,
        std::ref(m_static_mesh_buffer)
    ));

    // Load test geometry in the buffer
    // Load test geometry to the geometry buffers
    std::vector<Vertex3D> vertices = { 
        {{0., -0.5, 0.}, {1.,0.,0.}, {0., 0.}}, 
        {{0.5, 0.5, 0.}, {1.,1.,0.}, {0., 0.}}, 
        {{-0.5, 0.5, 0.}, {1.,0.,1.}, {0., 0.}}, 
    };
    std::vector<u32> indices = { 0, 1, 2 };

    m_device.geometryBufferLoad(
        m_static_mesh_buffer,
        vertices, 
        indices
    );
}

// Shutdown the renderer implementation
void VkRenderer::shutdown() 
{
    m_delete_queue.callDeleter();
}

// Resize the renderer viewport
void VkRenderer::resize(u32 width, u32 height) {
    if (width != 0 && height != 0) {
        recreateSwapChain(width, height);
        m_minimized = false;
    } else {
        m_minimized = true;
    }
}

// Recreate the swap chain and the swap chain render attachments
void VkRenderer::recreateSwapChain(u32 width, u32 height)
{
    destroySwapChainAttachment();
    
    m_swap_chain.reinitialize(m_context, m_device, width, height);

    createSwapChainAttachment();
}

// Set renderer v-sync 
void VkRenderer::setVsync(bool v_sync)
{
    destroySwapChainAttachment();
    
    m_swap_chain.setVsync(m_context, m_device, v_sync);
    
    createSwapChainAttachment();
}

// Toggle renderer v-sync
void VkRenderer::toggleVsync()
{
    bool v_status = m_swap_chain.vSync();
    setVsync(!v_status);
}

/*
 *
 *      Rendering code      
 *
 * */

// Draw and present a frame
void VkRenderer::draw()
{
    if (m_minimized)
        return;
    
    FrameData &frame_data = getCurrentFrame();

    vkWaitForFences(
        m_device.logical, 
        1, &frame_data.render_fence,
        VK_TRUE, UINT64_MAX
    );

    // If the swap chain is outdated recreate it
    if (m_swap_chain.outOfDate()) {
        VkExtent2D extent = m_swap_chain.extent();
        recreateSwapChain(extent.width, extent.height);
    }

    // Acquire the next swap chain image for rendering
    bool was_acquired = m_swap_chain.acquireNextImage(
        m_device,
        frame_data.image_semaphore,
        VK_NULL_HANDLE
    );

    if (!was_acquired)
        return;

    // Reset the rendering fence
    vkResetFences(m_device.logical, 1, &frame_data.render_fence);

    // Start render pass and command buffer recording
    frame_data.main_cmd_buffer.reset();
    frame_data.main_cmd_buffer.begin(false, false, false);

    RenderAttachment &attachment =
        m_swap_chain_attachements.at(m_swap_chain.currentImage());
    
    RenderPass::RenderArea render_area(
        m_swap_chain.extent().width,
        m_swap_chain.extent().height
    );
    
    m_main_render_pass.begin(
        attachment,
        render_area,
        frame_data.main_cmd_buffer
    );

    // TODO render code...

    // End command buffer recording and render pass
    m_main_render_pass.end(frame_data.main_cmd_buffer);
    frame_data.main_cmd_buffer.end();

    // Submit the rendering command buffer
    frame_data.main_cmd_buffer.submit(
        m_device.graphics_queue, 
        frame_data.render_fence, 
        1, &frame_data.image_semaphore, 
        1, &frame_data.render_semaphore, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

    // Present the swap chain image
    m_swap_chain.presentImage(m_device, frame_data.render_semaphore);
}

/*
 *
 *      Frame data functions
 *
 * */

// Create and initialized all the frame in flight data
void VkRenderer::createFrameDatas()
{
    u32 frame_in_flight = m_swap_chain.frameInFlight();
    m_frames_data.resize(frame_in_flight);

    for (auto& frame_data : m_frames_data) {
        // Create the sync object
        frame_data.render_fence = m_device.createFence(true);        
        frame_data.render_semaphore = m_device.createSemaphore();
        frame_data.image_semaphore = m_device.createSemaphore();

        // Create the command pool and buffers
        frame_data.graphics_cmd_pool = m_device.createCmdPool(
            Device::QueueType::Graphics,
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
        );

        frame_data.main_cmd_buffer = m_device.allocateCmdBuffer(
            frame_data.graphics_cmd_pool
        );
    }
}

// Destroy all the frame in flight data
void VkRenderer::destroyFrameData()
{
    vkDeviceWaitIdle(m_device.logical);
    
    for (auto& frame_data : m_frames_data) {
        // Destroy the sync object
        m_device.destroyFence(frame_data.render_fence);
        m_device.destroySemaphore(frame_data.render_semaphore);
        m_device.destroySemaphore(frame_data.image_semaphore);

        // Destroy command pool and buffers
        m_device.freeCmdBuffer(
            frame_data.graphics_cmd_pool,
            frame_data.main_cmd_buffer
        );

        m_device.destroyCmdPool(frame_data.graphics_cmd_pool);
    }
}

// Get the current frame in flight data
VkRenderer::FrameData& VkRenderer::getCurrentFrame()
{
    u32 current_frame = m_swap_chain.currentFrame();
    
    return m_frames_data.at(current_frame);
}

/*
 *
 *      Render attachment functions
 *
 * */

// Create the swap chain render attachments using the main render pass
void VkRenderer::createSwapChainAttachment()
{
    u32 attachments_count = m_swap_chain.imageCount();
    m_swap_chain_attachements.resize(attachments_count);

    // Create render attachments with the swap chain image views
    for (u32 i = 0; i < attachments_count; i++) {
        RenderAttachment& attachment = m_swap_chain_attachements.at(i);
        VkImageView view = m_swap_chain.imageViews().at(i);

        attachment = m_device.createRenderAttachment(
            m_main_render_pass,
            view,
            m_swap_chain.extent(),
            m_swap_chain.format()
        );
    }
}

// Destroy the swap chain render attachments
void VkRenderer::destroySwapChainAttachment()
{
    vkDeviceWaitIdle(m_device.logical);

    for (auto& attachment : m_swap_chain_attachements) {
        m_device.destroyRenderAttachment(attachment);
    }
}

} // namespace cndt::vulkan
