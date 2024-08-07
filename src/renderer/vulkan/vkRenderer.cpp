#include "renderer/vulkan/vkRenderer.h"
#include "conduit/assets/shader.h"
#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/graph/graph.h"
#include "renderer/vulkan/pipelines/vkShaderProgramBuilder.h"
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

    m_in_flight_count = 2;

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
        m_in_flight_count,
        m_frame_width,
        m_frame_height,
        true,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    );
    m_delete_queue.addDeleter([&](){
        m_swap_chain.shutdown();
    });

    // Create the main render pass
    RenderPass::Attachment color_attachments = { };
    color_attachments.format = m_swap_chain.format();
    color_attachments.samples = GpuImage::Info::Sample::Count_1;

    color_attachments.load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachments.store_op = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachments.stencil_load_op = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachments.stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachments.initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachments.final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    RenderPass::Dependency input_dep = { };
    input_dep.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    input_dep.src_access_mask = 0;
    input_dep.dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    input_dep.dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    input_dep.by_region = true;

    RenderPass::Dependency output_dep = { };
    output_dep.src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    output_dep.src_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    output_dep.dst_stage_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    output_dep.dst_access_mask = 0;

    VkAttachmentReference attachment_ref = { };
    attachment_ref.attachment = 0;
    attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    RenderPass::Subpass main_subpass = { };
    main_subpass.color_attachments.push_back(attachment_ref);

    m_main_render_pass = m_device.createRenderPass(
        { color_attachments },

        {input_dep, output_dep},
        {main_subpass}
    );
    m_delete_queue.addDeleter([&](){
        m_device.destroyRenderPass(m_main_render_pass);
    });

    // Create the frame in flight data
    createInFlightDatas(m_in_flight_count);
    m_delete_queue.addDeleter([&](){
        destroyInFlightData();
    });
    
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
    auto program_builder = getShaderProgramBuilder();

    program_builder->addStage(m_asset_manager.get<Shader>("builtinVert"));
    program_builder->addStage(m_asset_manager.get<Shader>("builtinFrag"));

    program_builder->configureInputVertex
        <Vertex3D, glm::vec3, glm::vec3, glm::vec2>
    (
        {0, &Vertex3D::position, ShaderProgram::Format::f32, 3},
        {1, &Vertex3D::color, ShaderProgram::Format::f32, 3},
        {2, &Vertex3D::text_coord, ShaderProgram::Format::f32, 2}
    );

    ShaderProgram::RasterConfig raster_config = { };
    program_builder->configureRasterizer(raster_config);

    m_graphics_pipeline = m_device.createGraphicsPipeline(
        m_main_render_pass,
        program_builder->build(),

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
    vkDeviceWaitIdle(m_device.logical());
    
    m_delete_queue.callDeleter();
}

// Resize the renderer viewport
void VkRenderer::resize(u32 width, u32 height) {
    if (width != 0 && height != 0) {
        m_frame_width = width;
        m_frame_height = height;
        
        m_minimized = false;
        
        m_swap_chain.setSurfaceExtent(width, height);
    } else {
        m_minimized = true;
    }
}

// Set renderer v-sync 
void VkRenderer::setVsync(bool v_sync)
{
    m_swap_chain.setVsync(v_sync);
}

// Toggle renderer v-sync
void VkRenderer::toggleVsync()
{
    bool v_status = m_swap_chain.vSync();
    setVsync(!v_status);
}

/*
 *
 *      Renderer getter function
 *
 * */

// Get a shader program builder
RenderRef<ShaderProgramBuilder> VkRenderer::getShaderProgramBuilder() 
{
    return RenderRef<ShaderProgramBuilder>(
        new VulkanShaderProgramBuilder(&m_device)
    );
}

/*
 *
 *      Rendering code      
 *
 * */

// Return a clear render graph ready to be built 
RenderGraph VkRenderer::getRenderGraph() 
{
    return RenderGraph();
}

// Execute the given render graph
void VkRenderer::executeGraph(RenderGraph& graph)
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
        sizeof(CameraModel), 0
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
    
    // Acquire the next swap chain image for rendering
    const VulkanImage* image = m_swap_chain.acquireNextImage(
        frame_data.image_semaphore,
        VK_NULL_HANDLE
    );

    if (image == nullptr)
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
    RenderPass::RenderArea render_area(
        m_swap_chain.extent().width,
        m_swap_chain.extent().height
    );

    VkClearValue clear = { };
    clear.color.float32[0] = 0.;
    clear.color.float32[1] = 0.;
    clear.color.float32[2] = 0.;
    clear.color.float32[3] = 1.;
    
    m_main_render_pass.begin(
        frame_data.main_cmd_buffer,
        render_area,

        { image },
        { clear }
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

    m_main_render_pass.cleanupRoutine();

    // Submit the rendering command buffer
    frame_data.main_cmd_buffer.submit(
        m_device.graphics_queue, 
        frame_data.render_fence.hande(), 
        1, &frame_data.image_semaphore, 
        1, &frame_data.render_semaphore, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );
}

// Present the current swap chain frame, return true if the frame was 
// presented successfully to the screen
void VkRenderer::presentFrame()
{
    InFlightData &frame_data = getCurrentInFlightData();
    
    // Present the swap chain image
    m_swap_chain.presentImage(frame_data.render_semaphore);

    // Increment the renderer frame count
    m_frame_count += 1;
}

/*
 *
 *      Frame data functions
 *
 * */

// Create and initialized all the frame in flight data
void VkRenderer::createInFlightDatas(u32 in_flight_count)
{
    m_in_flight_data.resize(in_flight_count);

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
        GpuBuffer::Info uniform_info = { };
        uniform_info.domain = GpuBuffer::Info::Domain::Host;
        uniform_info.usage = GpuBuffer::Info::Usage::UniformBuffer;
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
    vkDeviceWaitIdle(m_device.logical());
    
    for (auto& data : m_in_flight_data) {
        // Destroy the descriptor allocator
        m_device.destroyDescriptorAllocator(data.descriptor_allocator);

        // Unmap and destroy uniforms buffers
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
    u32 current_frame = m_frame_count % m_in_flight_count;
    
    return m_in_flight_data.at(current_frame);
}

} // namespace cndt::vulkan
