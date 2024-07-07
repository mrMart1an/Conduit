#include "renderer/vulkan/vkRenderer.h"
#include "renderer/vulkan/vkDevice.h"
#include "renderer/vulkan/vkUniformData.h"

#include <cstring>
#include <functional>
#include <vector>

#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

/*
 *
 *      Initialization code
 *
 * */

// Initialize the renderer implementation
void VkRenderer::initialize(
    EngineConfig::Renderer,
    const char *app_title,

    Window *window_p
) {
    // Store the swap chain attachments dimension
    m_frame_width = window_p->getWindowData().buffer_width;
    m_frame_height = window_p->getWindowData().buffer_height;

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
        m_frame_width,
        m_frame_height,
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
        { 0., 0., 0., 1.}
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
    createInFlightDatas();
    m_delete_queue.addDeleter(std::bind(
        &VkRenderer::destroyInFlightData,
        this
    ));
    
    // Create the uniform buffer descriptor set layout
    auto layout_builder = m_device.createDescriptorLayoutBuilder();
    layout_builder.addBinding(
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_SHADER_STAGE_VERTEX_BIT
    );

    m_uniform_layout = layout_builder.build();
    m_delete_queue.addDeleter([&]() {
        m_device.destroyDescriptorLayout(m_uniform_layout);
    });

    // Create a graphics pipeline
    m_graphics_pipeline = m_device.createGraphicsPipeline(
        m_main_render_pass,
        "resources/shaders/builtin.vert.spv",
        "resources/shaders/builtin.frag.spv",
        { m_uniform_layout.layout() }
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
        {{-0.5, -0.5, 0.}, {0.,1.,0.}, {0., 0.}}, 
        {{0.5, -0.5, 0.}, {0.,0.,1.}, {0., 0.}}, 
        {{0.5, 0.5, 0.}, {0.,1.,0.}, {0., 0.}}, 
        {{-0.5, 0.5, 0.}, {1.,0.,0.}, {0., 0.}}, 
    };
    std::vector<u32> indices = { 0, 1, 2, 0, 2, 3 };

    m_device.geometryBufferLoad(
        m_static_mesh_buffer,
        vertices, 
        indices
    );
}

// Shutdown the renderer implementation
void VkRenderer::shutdown() 
{
    vkDeviceWaitIdle(m_device.logical);
    
    m_delete_queue.callDeleter();
}

// Resize the renderer viewport
void VkRenderer::resize(u32 width, u32 height) {
    if (width != 0 && height != 0) {
        m_frame_width = width;
        m_frame_height = height;
        
        m_minimized = false;
        
        m_swap_chain.setOutOfDate();
    } else {
        m_minimized = true;
    }
}

// Recreate the swap chain and the swap chain render attachments
void VkRenderer::recreateSwapChain()
{
    vkDeviceWaitIdle(m_device.logical);
    
    destroySwapChainAttachment();
    
    m_swap_chain.reinitialize(
        m_context,
        m_device,
        m_frame_width,
        m_frame_height
    );

    createSwapChainAttachment();
}

// Set renderer v-sync 
void VkRenderer::setVsync(bool v_sync)
{
    vkDeviceWaitIdle(m_device.logical);
    
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
    if (!beginFrame()) 
        return; 
    
    InFlightData &frame_data = getCurrentInFlightData();

    // Descriptor set
    auto descriptor_set = frame_data.descriptor_allocator.allocate(
        m_uniform_layout
    );
    frame_data.descriptor_writer.updateSet(descriptor_set);

    CameraModel cam = {};
    cam.model = glm::rotate(
        glm::mat4(1.0f),
        (float)((m_frame_count % 240)) / 240.f * glm::radians(360.f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    cam.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    cam.proj = glm::perspectiveZO(
        glm::radians(45.0f),
        m_frame_width / (float) m_frame_height,
        0.1f, 10.0f
    );
    cam.proj[1][1] *= -1;
    
    frame_data.camera_model_uniform.copyMemToBuf(
        &cam, 
        0, sizeof(CameraModel)
    );
    
    // Test triangle code
    m_graphics_pipeline.bind(frame_data.main_cmd_buffer);

    // Bind the descriptor
    vkCmdBindDescriptorSets(
        frame_data.main_cmd_buffer.handle(), 
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        m_graphics_pipeline.layout(),
        0, 1, 
        &descriptor_set,
        0, nullptr
    );
    
    VkDeviceSize offsets[1] = { 0 };
    VkBuffer vertex_buf = m_static_mesh_buffer.vertex().handle(); 
    
    vkCmdBindVertexBuffers(
        frame_data.main_cmd_buffer.handle(), 
        0, 1,
        &vertex_buf, 
        offsets
    );
    
    vkCmdBindIndexBuffer(
        frame_data.main_cmd_buffer.handle(),
        m_static_mesh_buffer.index().handle(),
        0,
        VK_INDEX_TYPE_UINT32
    );
    
    // Issue draw call
    vkCmdDrawIndexed(
        frame_data.main_cmd_buffer.handle(),
        6,
        1,
        0, 0, 0
    );

    endFrame();
    presentFrame();
}

// Begin frame rendering, return true if the frame was begun successfully 
// and the rendering can proceed
bool VkRenderer::beginFrame()
{
    if (m_minimized)
        return false;
    
    InFlightData &frame_data = getCurrentInFlightData();
    
    frame_data.render_fence.wait();
    
    // If the swap chain is outdated recreate it
    if (m_swap_chain.outOfDate()) {
        recreateSwapChain();
        return false;
    }
    
    // Acquire the next swap chain image for rendering
    bool was_acquired = m_swap_chain.acquireNextImage(
        m_device,
        frame_data.image_semaphore,
        VK_NULL_HANDLE
    );

    if (!was_acquired)
        return false;
    
    // Reset the rendering fence and descriptor allocator
    frame_data.render_fence.reset();
    frame_data.descriptor_allocator.clearPools();
        
    // Reset the main command buffer and start recording
    frame_data.main_cmd_buffer.reset();
    frame_data.main_cmd_buffer.begin(false, false, false);
    
    // Setup viewport and scissor
    VkViewport viewport = { };
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = m_swap_chain.extent().width;
    viewport.height = m_swap_chain.extent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(frame_data.main_cmd_buffer.handle(), 0, 1, &viewport);
    
    VkRect2D scissor = { };
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = m_swap_chain.extent();
    vkCmdSetScissor(frame_data.main_cmd_buffer.handle(), 0, 1, &scissor);
    
    // Start the main render pass on the current swap chain attachment
    RenderAttachment &attachment = getCurrentAttachment();
    
    RenderPass::RenderArea render_area(
        m_swap_chain.extent().width,
        m_swap_chain.extent().height
    );
    
    m_main_render_pass.begin(
        attachment,
        render_area,
        frame_data.main_cmd_buffer
    );

    return true;
}

// End frame rendering
void VkRenderer::endFrame()
{
    InFlightData &frame_data = getCurrentInFlightData();
    
    // End command buffer recording and render pass
    m_main_render_pass.end(frame_data.main_cmd_buffer);
    frame_data.main_cmd_buffer.end();

    // Submit the rendering command buffer
    frame_data.main_cmd_buffer.submit(
        m_device.graphics_queue, 
        frame_data.render_fence.hande(), 
        1, &frame_data.image_semaphore, 
        1, &frame_data.render_semaphore, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );

    // Increment the renderer frame count
    m_frame_count += 1;
}

// Present the current swap chain frame, return true if the frame was 
// presented successfully to the screen
bool VkRenderer::presentFrame()
{
    InFlightData &frame_data = getCurrentInFlightData();
    
    // Present the swap chain image
    return m_swap_chain.presentImage(m_device, frame_data.render_semaphore);
}

/*
 *
 *      Frame data functions
 *
 * */

// Create and initialized all the frame in flight data
void VkRenderer::createInFlightDatas()
{
    u32 frame_in_flight = m_swap_chain.frameInFlight();
    m_in_flight_data.resize(frame_in_flight);

    for (auto& data : m_in_flight_data) {
        // Create the sync object
        data.render_fence = m_device.createFence(true);        
        data.render_semaphore = m_device.createSemaphore();
        data.image_semaphore = m_device.createSemaphore();

        // Create the command pool and buffers
        data.graphics_cmd_pool = m_device.createCmdPool(
            Device::QueueType::Graphics,
            false, true, false
        );

        data.main_cmd_buffer = 
            data.graphics_cmd_pool.allocateCmdBuffer();

        // Create and map the uniforms buffers
        GpuBufferInfo uniform_info = { };
        uniform_info.domain = GpuBufferInfo::Domain::Host;
        uniform_info.usage = GpuBufferInfo::Usage::UniformBuffer;
        uniform_info.size = sizeof(CameraModel);

        data.camera_model_uniform = m_device.createBuffer(uniform_info);
        
        // Create the descriptor allocator
        std::vector<DescriptorAllocator::PoolSizeRatio> ratio = {
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}  
        };
        data.descriptor_allocator = m_device.createDescriptorAllocator(ratio);

        // Create descriptor writer
        data.descriptor_writer = m_device.createDescriptorWriter();
        data.descriptor_writer.writeBuffer(
            0, 
            data.camera_model_uniform.handle(), 
            0,
            sizeof(CameraModel),
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        );
    }
}

// Destroy all the frame in flight data
void VkRenderer::destroyInFlightData()
{
    vkDeviceWaitIdle(m_device.logical);
    
    for (auto& data : m_in_flight_data) {
        // Destroy the descriptor allocator
        m_device.destroyDescriptorAllocator(data.descriptor_allocator);

        // Unmap and destroy uniforms buffers
        data.camera_model_uniform.unmap();
        m_device.destroyBuffer(data.camera_model_uniform);

        // Destroy the sync object
        m_device.destroyFence(data.render_fence);
        m_device.destroySemaphore(data.render_semaphore);
        m_device.destroySemaphore(data.image_semaphore);

        // Destroy command pool and buffers
        data.graphics_cmd_pool.freeCmdBuffer(
            data.main_cmd_buffer
        );

        m_device.destroyCmdPool(data.graphics_cmd_pool);

        data = InFlightData();
    }
}

// Get the current frame in flight data
VkRenderer::InFlightData& VkRenderer::getCurrentInFlightData()
{
    u32 current_frame = m_swap_chain.currentFrame();
    
    return m_in_flight_data.at(current_frame);
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
    for (auto& attachment : m_swap_chain_attachements) {
        m_device.destroyRenderAttachment(attachment);
    }
}

// Get the current swap chain render attachment
RenderAttachment& VkRenderer::getCurrentAttachment()
{
    return m_swap_chain_attachements.at(m_swap_chain.currentImage());
}

} // namespace cndt::vulkan
