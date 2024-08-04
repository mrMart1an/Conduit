#include "conduit/assets/handle.h"
#include "conduit/assets/shader.h"

#include "conduit/defines.h"
#include "conduit/logging.h"
#include "conduit/internal/core/deleteQueue.h"

#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

#include "renderer/vulkan/descriptor/vkDescriptorLayout.h"
#include "renderer/vulkan/pipelines/vkShaderProgram.h"
#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkDevice.h"
#include "renderer/vulkan/storage/vkImage.h"
#include "renderer/vulkan/vkCommandPool.h"

#include <array>
#include <cstring>
#include <functional>
#include <string_view>
#include <vector>

#include "vk_mem_alloc.h"
#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Initialize a vulkan device
void Device::initialize(
    Context *context_p,
    PhysicalDeviceRequirement physical_device_requirement
) {
    log::core::info("Creating vulkan device");

    m_allocator = context_p->allocator;

    // Pick a suitable physical device matching the requirement
    pickPhysicalDevice(context_p, physical_device_requirement);

    // Create the logical device
    createLogicalDevice(context_p);
    m_delete_queue.addDeleter(std::bind(&Device::destroyLogicalDevice, this));

    // Initialize the vma allocator
    initializeVmaAllocator(context_p->instance);
    m_delete_queue.addDeleter([&]() {
        shutdownVmaAllocator();
    });

    // Retrieve the device queue
    retrieveQueue();

    // Create transfer command pool and buffer and fence
    if (m_device_requirement.required_queue.transfer()) {
        // Create the transient command pool for transfer operation
        m_transfer_cmd_pool = createCmdPool(
            QueueType::Transfer, true, true
        );
        m_transfer_cmd_buf = m_transfer_cmd_pool.allocateCmdBuffer();

        m_delete_queue.addDeleter([&]() {
            destroyCmdPool(m_transfer_cmd_pool);
        });
        
        m_delete_queue.addDeleter([&]() {
            m_transfer_cmd_pool.freeCmdBuffer(m_transfer_cmd_buf);
        });
    }

    // Create compute command pool and buffer and fence
    if (m_device_requirement.required_queue.compute()) {
        // Create the transient command pool for transfer operation
        m_compute_cmd_pool = createCmdPool(
            QueueType::Compute, true, true
        );
        m_compute_cmd_buf = m_compute_cmd_pool.allocateCmdBuffer();

        m_delete_queue.addDeleter([&]() {
            destroyCmdPool(m_compute_cmd_pool);
        });
        
        m_delete_queue.addDeleter([&]() {
            m_compute_cmd_pool.freeCmdBuffer(m_compute_cmd_buf);
        });
    }

    // Create transfer command pool and buffer and fence
    if (m_device_requirement.required_queue.graphics()) {
        // Create the transient command pool for transfer operation
        m_graphics_cmd_pool = createCmdPool(
            QueueType::Graphics, true, true
        );
        m_graphics_cmd_buf = m_graphics_cmd_pool.allocateCmdBuffer();

        m_delete_queue.addDeleter([&]() {
            destroyCmdPool(m_graphics_cmd_pool);
        });

        m_delete_queue.addDeleter([&]() {
            m_graphics_cmd_pool.freeCmdBuffer(m_graphics_cmd_buf);
        });
    }

    // Create the immediate command fence
    m_immediate_fence = createFence(false);

    m_delete_queue.addDeleter([&]() {
        destroyFence(m_immediate_fence);
    });

    // Set initial buffer and image id
    m_next_buffer_id = 0;
    m_next_image_id = 0;
}

// Shutdown vulkan device
void Device::shutdown()
{
    log::core::info("Destroying vulkan device");

    // Empty the delete queue
    m_delete_queue.callDeleter();
}

/*
 *
 *      Render pass functions
 *
 * */

// Create a vulkan render pass
RenderPass Device::createRenderPass(
    VkFormat attachment_format,
    RenderPass::ClearColor clear_color
) {
    RenderPass out_pass(clear_color);

    // Color attachment
    VkAttachmentDescription color_attachment = { };
    color_attachment.format = attachment_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference color_attachment_ref = { };
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    // Main sub pass 
    VkSubpassDescription subpass = { };
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    
    // Sub pass dependency
    VkSubpassDependency dependency = { };
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    // Render pass
    VkRenderPassCreateInfo render_pass_info = { };
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VkResult res = vkCreateRenderPass(
        logical,
        &render_pass_info,
        m_allocator,
        &out_pass.m_handle
    );
    
    if (res != VK_SUCCESS) {
        throw RenderPassCreationError(
            "Render pass creation error {}",
            vk_error_str(res)
        );
    }

    return out_pass;
}

// Destroy render pass
void Device::destroyRenderPass(RenderPass &render_pass)
{
    vkDestroyRenderPass(
        logical,
        render_pass.m_handle,
        m_allocator
    );

    render_pass.m_handle = VK_NULL_HANDLE;
    render_pass.m_clear_color = RenderPass::ClearColor();
}

/*
 *
 *      Render attachment functions
 *
 * */

// Create a vulkan render attachment
RenderAttachment Device::createRenderAttachment(
    RenderPass render_pass,
    
    VkImageView image_view,
    VkExtent2D image_extent,
    VkFormat image_format
) {
    RenderAttachment out_attachment;

    // Store extent and format of the attachment
    out_attachment.m_extent = image_extent;
    out_attachment.m_format = image_format;

    VkFramebufferCreateInfo frame_buffer_info = { };
    frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    
    frame_buffer_info.attachmentCount = 1;
    frame_buffer_info.pAttachments = &image_view;
    frame_buffer_info.renderPass = render_pass.m_handle; 
    
    frame_buffer_info.width = image_extent.width;
    frame_buffer_info.height = image_extent.height;
    frame_buffer_info.layers = 1;

    VkResult res = vkCreateFramebuffer(
        logical,
        &frame_buffer_info,
        m_allocator,
        &out_attachment.m_frame_buffer
    ); 
    
    if (res != VK_SUCCESS) {
        throw RenderAttachmentCreationError(
            "frame buffer creation error {}",
            vk_error_str(res)
        );
    }

    return out_attachment;
}

// Create a vulkan render attachment
RenderAttachment Device::createRenderAttachment(
    RenderPass render_pass,
    VulkanImage &image
) {
    VkExtent2D extent = { };
    extent.width = image.m_info.extent.width;
    extent.height = image.m_info.extent.height;

    return createRenderAttachment(
        render_pass,
        
        image.m_view,
        extent,
        image.m_vk_format
    );
}

// Destroy the given vulkan render attachment
void Device::destroyRenderAttachment(RenderAttachment &attachment)
{
    vkDestroyFramebuffer(
        logical,
        attachment.m_frame_buffer,
        m_allocator
    );

    attachment = RenderAttachment();
}

/*
 *
 *      Sync object functions
 *
 * */

// Create a fence, used for GPU to CPU synchronization 
Fence Device::createFence(bool signaled)
{
    Fence out_fence;
    out_fence.m_device_p = this;

    VkFenceCreateInfo fence_info = { };
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkResult res = vkCreateFence(
        logical, 
        &fence_info, 
        m_allocator, 
        &out_fence.m_handle
    );
    
    if (res != VK_SUCCESS) {
        throw FenceInitError(
            "Vulkan fence initialization error {}",
            vk_error_str(res)
        );
    }

    return out_fence;
}

// Destroy the given fence
void Device::destroyFence(Fence &fence)
{
    vkDestroyFence(logical, fence.m_handle, m_allocator);

    fence = Fence();
}

// Create a semaphore, used for GPU to GPU synchronization
VkSemaphore Device::createSemaphore()
{
    VkSemaphore out_semaphore;
    
    VkSemaphoreCreateInfo semaphore_info = { };
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult res = vkCreateSemaphore(
        logical, 
        &semaphore_info, 
        m_allocator, 
        &out_semaphore
    );
    
    if (res != VK_SUCCESS) {
        throw SemaphoreInitError(
            "Vulkan semaphore initialization error {}",
            vk_error_str(res)
        );
    }

    return out_semaphore;
}

// Destroy the given semaphore
void Device::destroySemaphore(VkSemaphore &semaphore) 
{
    vkDestroySemaphore(logical, semaphore, m_allocator);
}

/*
 *
 *      Image functions
 *
 * */

// Create a new image for 
VulkanImage Device::createImage(const GpuImage::Info& info)
{
    VulkanImage out_image;

    out_image.m_device_p = this;
    out_image.m_info = info;
    out_image.m_vk_format = getVkFormat(info.format);

    // Assign image id
    out_image.m_id = m_next_image_id;
    m_next_image_id += 1;
    
    // Prepare the create info struct
    VkImageCreateInfo image_info = { };
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = info.extent.width;
    image_info.extent.height = info.extent.height;
    image_info.extent.depth = 1;
    image_info.arrayLayers = 1;
    image_info.format = out_image.m_vk_format;

    // Calculate number of level for the mipmap chain
    if (info.store_mipmap) {
        out_image.m_mipmap_levels = static_cast<uint32_t>(
            std::floor(std::log2(std::max(
                info.extent.width, 
                info.extent.height
            )))) + 1;

        image_info.mipLevels = out_image.m_mipmap_levels;
    } else {
        out_image.m_mipmap_levels = 1;
        image_info.mipLevels = 1;
    }

    // Always use optimal tiling
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;

    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = getVkImageUsage(info.usage);

    image_info.samples = getVkSampleCount(info.sampe);

    // TODO move to exclusive mode once the render graph system is ready
    image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;

    u32 unique_queue_count = 1;
    u32 queue_indecies_p[3];

    queue_indecies_p[0] = m_queue_indices.graphicsIndex();

    if (queue_indecies_p[0] != m_queue_indices.computeIndex()) {
        queue_indecies_p[1] = m_queue_indices.computeIndex();
        unique_queue_count += 1;
    }

    if (
        queue_indecies_p[0] != m_queue_indices.transferIndex() &&
        queue_indecies_p[1] != m_queue_indices.transferIndex()
    ) {
        queue_indecies_p[2] = m_queue_indices.transferIndex();
        unique_queue_count += 1;
    }

    image_info.queueFamilyIndexCount = unique_queue_count;
    image_info.pQueueFamilyIndices = queue_indecies_p;

    // Allocation create info
    VmaAllocationCreateInfo alloc_create_info = { };
    alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;

    // Create the image handle and allocate memory with VMA
    VkResult res = vmaCreateImage(
        m_vma_allocator, 
        &image_info, 
        &alloc_create_info,
        &out_image.m_handle,
        &out_image.m_allocation,
        &out_image.m_alloc_info
    );

    if (res != VK_SUCCESS) {
        throw ImageCreateError(
            "Image handle creation error: {}",
            vk_error_str(res)
        );
    }

    // Create image view
    VkImageViewCreateInfo view_info = { };
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.image = out_image.m_handle;
    view_info.format = out_image.m_vk_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = out_image.m_mipmap_levels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkResult view_res = vkCreateImageView(
        logical,
        &view_info,
        m_allocator,
        &out_image.m_view
    );

    if (view_res != VK_SUCCESS) {
        throw ImageCreateError(
            "image view creation error {}",
            vk_error_str(view_res)
        );
    }
    
    return out_image;
}

// Destroy the given image
void Device::destroyImage(VulkanImage &image)
{
    vkDestroyImageView(logical, image.m_view, m_allocator);
    vkDestroyImage(logical, image.m_handle, m_allocator);
    
    image = VulkanImage();
}

/*
 *
 *     Buffer functions
 *
 * */

// Create a new buffer with the given requirement
VulkanBuffer Device::createBuffer(const GpuBuffer::Info& info)
 {
    VulkanBuffer out_buffer;

    out_buffer.m_device_p = this;
    out_buffer.m_info = info;
    
    out_buffer.m_mapped = false;

    // Assign buffer id
    out_buffer.m_id = m_next_buffer_id;
    m_next_buffer_id += 1;

    // Create the buffer
    VkBufferCreateInfo buffer_info = { };
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = info.size;

    // TODO move to exclusive mode once the render graph system is ready
    buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;

    u32 unique_queue_count = 1;
    u32 queue_indecies_p[3];

    queue_indecies_p[0] = m_queue_indices.graphicsIndex();

    if (queue_indecies_p[0] != m_queue_indices.computeIndex()) {
        queue_indecies_p[1] = m_queue_indices.computeIndex();
        unique_queue_count += 1;
    }

    if (
        queue_indecies_p[0] != m_queue_indices.transferIndex() &&
        queue_indecies_p[1] != m_queue_indices.transferIndex()
    ) {
        queue_indecies_p[2] = m_queue_indices.transferIndex();
        unique_queue_count += 1;
    }

    buffer_info.queueFamilyIndexCount = unique_queue_count;
    buffer_info.pQueueFamilyIndices = queue_indecies_p;

    // Get buffer usage
    buffer_info.usage = getVkBufferUsage(info.usage);

    // Allocation info
    VmaAllocationCreateInfo alloc_create_info = { };

    if (info.domain == GpuBuffer::Info::Domain::Device) {
        alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    } else if (info.domain == GpuBuffer::Info::Domain::Host) {
        alloc_create_info.usage = VMA_MEMORY_USAGE_AUTO;

        // Always create host visible memory with the create mapped flag
        alloc_create_info.flags =
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;
        
    } else if (info.domain == GpuBuffer::Info::Domain::HostCached) {
        alloc_create_info.requiredFlags = 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

        // Always create host visible memory with the create mapped flag
        alloc_create_info.flags =
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;

    } else if (info.domain == GpuBuffer::Info::Domain::HostCoherent) {
        alloc_create_info.requiredFlags = 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        // Always create host visible memory with the create mapped flag
        alloc_create_info.flags =
            VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT |
            VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    // Create the buffer and the allocation
    VkResult res = vmaCreateBuffer(
        m_vma_allocator,
        &buffer_info,
        &alloc_create_info,
        &out_buffer.m_handle,
        &out_buffer.m_allocation,
        &out_buffer.m_allocation_info
    );

    if (res != VK_SUCCESS) {
        throw BufferCreateError(
            "Vulkan buffer create error: {}",
            vk_error_str(res)
        );
    }

    return out_buffer;
}

// Destroy the given buffer
void Device::destroyBuffer(VulkanBuffer &buffer) 
{
    vmaDestroyBuffer(
        m_vma_allocator,
        buffer.m_handle,
        buffer.m_allocation
    );

    buffer = VulkanBuffer();
}

// Copy the content of one buffer to another
// this operation is blocking
void Device::copyBuffer(
    VkDeviceSize src_offset,
    const VulkanBuffer &src_buffer,
    
    VkDeviceSize dest_offset,
    VulkanBuffer &dest_buffer,

    VkDeviceSize size
) {
    copyBuffer(
        src_offset,
        src_buffer.m_handle,

        dest_offset,
        dest_buffer.m_handle,

        size
    );
} 

// Copy the content of one buffer to another
// this operation is blocking
void Device::copyBuffer(
    VkDeviceSize src_offset,
    VkBuffer src_buffer,
    
    VkDeviceSize dest_offset,
    VkBuffer dest_buffer,

    VkDeviceSize size
) {
    runCmdImmediate(
        QueueType::Transfer,
        [&](VkCommandBuffer cmd_buf) {
            VkBufferCopy copy_op = { };
            copy_op.size = size;
            copy_op.srcOffset = src_offset;
            copy_op.dstOffset = dest_offset;

            vkCmdCopyBuffer(
                cmd_buf, 
                src_buffer, 
                dest_buffer, 
                1, &copy_op
            );
        }
    );
}

/*
 *
 *      Descriptor set functions
 *
 * */

// Create the descriptor layout builder
DescriptorLayoutBuilder Device::createDescriptorLayoutBuilder()
{
    DescriptorLayoutBuilder out_builder;
    out_builder.m_device_p = this;

    return out_builder;
}
    
// Destroy the given descriptor set layout
void Device::destroyDescriptorLayout(DescriptorLayout &layout)
{
    vkDestroyDescriptorSetLayout(
        logical,
        layout.layout(),
        m_allocator
    );
    
    layout = DescriptorLayout();
}

// Create a descriptor set allocator
DescriptorAllocator Device::createDescriptorAllocator(
    std::vector<DescriptorAllocator::PoolSizeRatio> pool_ratio
) {
    DescriptorAllocator out_allocator = { };
    out_allocator.m_device_p = this;
    out_allocator.m_ratios = pool_ratio;

    out_allocator.m_in_use_pool = out_allocator.createPool();

    return out_allocator;
}

// Destroy a descriptor allocator
void Device::destroyDescriptorAllocator(DescriptorAllocator &allocator)
{
    allocator.destroyPools();
    
    allocator = DescriptorAllocator();
}

// Create a descriptor set writer
DescriptorWriter Device::createDescriptorWriter()
{
    DescriptorWriter out_writer = { };
    out_writer.m_device_p = this;
    
    return out_writer;
}

// PRIVATE ----------------------------

/*
 *
 *      Vulkan memory allocator functions
 *
 * */

// Initialize the device vma allocator
void Device::initializeVmaAllocator(VkInstance instance)
{
    VmaAllocatorCreateInfo allocator_info = {};

    allocator_info.instance = instance;
    allocator_info.physicalDevice = physical;
    allocator_info.device = logical;

    allocator_info.pHeapSizeLimit = nullptr;
    allocator_info.pAllocationCallbacks = nullptr;
    allocator_info.pDeviceMemoryCallbacks = nullptr;

    allocator_info.flags = 0;

    // Create the allocator and check the result
    VkResult res = vmaCreateAllocator(&allocator_info, &m_vma_allocator);

    if (res != VK_SUCCESS) {
        throw DeviceInitError(
            "Vma allocar initialization failed: {}",
            vk_error_str(res)
        );
    }
}

// Shutdown the device vma allocator
void Device::shutdownVmaAllocator()
{
    vmaDestroyAllocator(m_vma_allocator);
}

/*
 *
 *      Logical device functions
 *
 * */

// Create the logical device
void Device::createLogicalDevice(Context *context_p)
{
    QueueFamilyIndices indices = getQueueIndices(context_p, physical);

    // Store the physical device queues indices
    m_queue_indices = indices;
    
    // Queue creation info
    #define MAX_QUEUE_COUNT 4

    // If the queue have the same index we only need one queue create info
    // find the number of queue with unique indices
    u32 queue_count = 0;
    u32 queue_indices[MAX_QUEUE_COUNT];

    for (u32 i = 0; i < MAX_QUEUE_COUNT; i++) {
        u32 index = indices.indices()[i];
        
        if (index != (u32)-1) {
            // Check if the index is already in the queue indices array
            bool index_unique = true;
            
            for (u32 j = 0; j < queue_count; j++) {
                if (index == queue_indices[j]) {
                    index_unique = false;
                    break;
                }
            }

            if (index_unique) {
                queue_indices[queue_count] = index;
                queue_count += 1;
            }
        }
    }

    // Set up all the event queue info
    VkDeviceQueueCreateInfo queue_create_infos[MAX_QUEUE_COUNT] = {};
    const f32 queue_priority = 1.0f;

    for (usize i = 0; i < queue_count; i++) {
        queue_create_infos[i].sType = 
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = queue_indices[i];
        queue_create_infos[i].pQueuePriorities = &queue_priority;
        queue_create_infos[i].queueCount = 1;
    }

    // Create device info
    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos;
    create_info.queueCreateInfoCount = queue_count;
    create_info.pEnabledFeatures = &m_device_requirement.required_feature;

    // Specify the required device extensions
    std::vector<const char*> extensions =
        m_device_requirement.required_device_extensions.getExtensions();
    
    create_info.enabledExtensionCount = (u32)extensions.size();
    create_info.ppEnabledExtensionNames = extensions.data(); 

    // Add validation layers in debug builds
    #ifdef VK_VALIDATION
    std::vector<const char*> validation_layers = Validation::getLayers();
    
    // Check if the validation layers are valid
    create_info.enabledLayerCount = (u32)validation_layers.size();
    create_info.ppEnabledLayerNames = validation_layers.data();
    #endif
    
    // Create the logical device
    VkResult res = vkCreateDevice(
        physical, 
        &create_info, 
        m_allocator, 
        &logical
    );

    if (res != VK_SUCCESS) {
        throw DeviceInitError(
            "vkCreateDevice error: {}",
            vk_error_str(res)
        );
    }
}

// Destroy the logical device
void Device::destroyLogicalDevice()
{
    // Wait for all operation to end on the device
    vk_check(vkDeviceWaitIdle(logical));
    
    // Destroy the logical device
    vkDestroyDevice(logical, m_allocator);
}

/*
 *
 *      Shader module functions
 *
 * */

// Create a shader module for the required stage from the SPIR-V file
VulkanShaderModule Device::createShaderModule(AssetHandle<Shader> shader) 
{
    VulkanShaderModule out_shader;

    // Prepare the shader create info
    VkShaderModuleCreateInfo create_info = { };
        
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode = shader->getSpv(
        &create_info.codeSize
    );
    
    // Create the shader module
    VkResult res = vkCreateShaderModule(
        logical,
        &create_info,
        m_allocator,
        &out_shader.m_handle
    );

    if (res != VK_SUCCESS) {
        throw ShaderModuleFileError(
            "Vulkan shader module creation error ({}), name: {}",
            vk_error_str(res),
            shader.info().assetName()
        );
    }
    
    // Store the shader type 
    VkShaderStageFlagBits stage;

    switch (shader->type()) {
        case Shader::Type::Compute: {
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        }
        case Shader::Type::Vertex: {
            stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        }
        case Shader::Type::Fragment: {
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        }
        case Shader::Type::TessellationEval: {
            stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            break;
        }
        case Shader::Type::TessellationControl: {
            stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            break;
        }
        case Shader::Type::Geometry: {
            stage = VK_SHADER_STAGE_GEOMETRY_BIT;
            break;
        }

        default: {
            throw ShaderModuleCreateError(
                "Vulkan shader module create error: Invalid shader type"
            );
        }
    }

    // Stage create info
    out_shader.m_stage_create_info = { };
    out_shader.m_stage_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    
    out_shader.m_stage_create_info.stage = stage;
    out_shader.m_stage_create_info.module = out_shader.m_handle;
    
    // Use main as default shader entry point
    out_shader.m_stage_create_info.pName = "main"; 

    return out_shader;
}

// Destroy a shader module
void Device::destroyShaderModule(
    VulkanShaderModule &module
) {
    vkDestroyShaderModule(logical, module.m_handle, m_allocator);
    module = VulkanShaderModule();
}

/*
 *
 *      Queue functions
 *
 * */

// Retrieve the queue from the logical device
void Device::retrieveQueue()
{
    // Graphic queue
    if (m_device_requirement.required_queue.graphics()) {
        vkGetDeviceQueue(
            logical, 
            m_queue_indices.graphicsIndex(), 
            0, 
            &graphics_queue
        );
    }
    
    // Compute queue
    if (m_device_requirement.required_queue.compute()) {
        vkGetDeviceQueue(
            logical, 
            m_queue_indices.computeIndex(), 
            0, 
            &compute_queue
        );
    }
    
    // Transfer queue
    if (m_device_requirement.required_queue.transfer()) {
        vkGetDeviceQueue(
            logical, 
            m_queue_indices.transferIndex(), 
            0, 
            &transfer_queue
        );
    }
    
    // Present queue
    if (m_device_requirement.required_queue.present()) {
        vkGetDeviceQueue(
            logical,
            m_queue_indices.presentIndex(),
            0,
            &present_queue
        );
    }
}

/*
 *
 *      Command pool functions
 *
 * */

// Create the command pool  
CommandPool Device::createCmdPool(
    QueueType queue_type,

    bool transient_pool,
    bool reset_cmd_buffer,
    bool protected_cmd_buffer
) {
    // Calculate the queue index
    u32 index;
    switch (queue_type) {
        case QueueType::Graphics: {
            index = m_queue_indices.graphicsIndex();
            break;
        } 
        case QueueType::Compute: {
            index = m_queue_indices.computeIndex();
            break;
        } 
        case QueueType::Transfer: {
            index = m_queue_indices.transferIndex();
            break;
        } 
    }

    // Generate the pool flags
    VkCommandPoolCreateFlags flags = 0;

    if (transient_pool)
        flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (reset_cmd_buffer)
        flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (protected_cmd_buffer)
        flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;

    // Set the command pool resettable member
    CommandPool cmd_pool = createCmdPool(index, flags); 
    cmd_pool.m_ressetable_cmd_buffer = reset_cmd_buffer;

    return cmd_pool;
}

// Destroy the given command pool
void Device::destroyCmdPool(CommandPool cmd_pool)
{
    vkDestroyCommandPool(
        logical, 
        cmd_pool.m_handle, 
        m_allocator
    );

    cmd_pool = CommandPool();
}

/*
 *
 *      Immediate command functions
 *
 * */

// Execute the command in the given function and wait 
// for them to complete on the CPU
void Device::runCmdImmediate(
    QueueType type,
    std::function<void(VkCommandBuffer)> immediate_fun
) {
    // Select the queue and buffer for the immediate command type
    CommandBuffer cmd_buf;

    VkQueue queue;

    if (type == QueueType::Transfer) {
        cmd_buf = m_transfer_cmd_buf;
        queue = transfer_queue;

    } else if (type == QueueType::Graphics) {
        cmd_buf = m_graphics_cmd_buf;
        queue = graphics_queue;

    } else if (type == QueueType::Compute) {
        cmd_buf = m_compute_cmd_buf;
        queue = compute_queue;
    }

    // Record the immediate command
    cmd_buf.begin();
    cmd_buf.record(immediate_fun);
    cmd_buf.end();

    // Submit the command buffer to the queue 
    m_immediate_fence.reset();
    cmd_buf.submit(queue, m_immediate_fence.hande());

    // Wait for the fence to signal and reset the command buffer 
    m_immediate_fence.wait();

    cmd_buf.reset();
}

/*
 *
 *      Pipeline functions
 *
 * */

// Create a vulkan graphics pipeline
GraphicsPipeline Device::createGraphicsPipeline(
	RenderPass &render_pass,
    RenderRef<ShaderProgram> program_ref,

	std::vector<VkDescriptorSetLayout> descriptor_set_layout
) {
    GraphicsPipeline out_pipeline;

    VulkanShaderProgram &program = 
        static_cast<VulkanShaderProgram&>(*program_ref);

    // Enable dynamic viewport and scissor
    std::array<VkDynamicState, 2> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = { };
    dynamic_state.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = dynamic_states.size();
    dynamic_state.pDynamicStates = dynamic_states.data();

    // Viewport and scissor state 
    VkPipelineViewportStateCreateInfo viewport_state = { };
    viewport_state.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
   
    // Color blend attachment
    VkPipelineColorBlendAttachmentState color_blend_attachment = { };
    color_blend_attachment.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT | 
        VK_COLOR_COMPONENT_G_BIT | 
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;

    color_blend_attachment.blendEnable = VK_FALSE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; 
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;

    // Color blend create info
    VkPipelineColorBlendStateCreateInfo color_blending = { };
    color_blending.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f; 
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f; 
    color_blending.blendConstants[3] = 0.0f;

    // Vertex attribute and descriptor
    VkPipelineVertexInputStateCreateInfo vertex_input_info = { };
    vertex_input_info.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = 
        program.getVertexBindingDescription();
    
    vertex_input_info.pVertexAttributeDescriptions = 
        program.getVertexAttributeDescription(
            &vertex_input_info.vertexAttributeDescriptionCount
        );

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo input_assembly = { };
    input_assembly.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    
    // Pipeline layout
    VkPipelineLayoutCreateInfo pipeline_layout_info = { };
    pipeline_layout_info.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    pipeline_layout_info.setLayoutCount = descriptor_set_layout.size();
    pipeline_layout_info.pSetLayouts = descriptor_set_layout.data();

    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = VK_NULL_HANDLE;

    // Create pipeline layout
    VkResult res_layout = vkCreatePipelineLayout(
        logical, 
        &pipeline_layout_info, 
        m_allocator,
        &out_pipeline.m_layout
    );
    
    if (res_layout != VK_SUCCESS) {
        throw PipelineCreationError(
            "Pipeline layout creation error: {}",
            vk_error_str(res_layout)
        );
    }
    
    // Create the pipeline info
    VkGraphicsPipelineCreateInfo pipeline_create_info = { };
    pipeline_create_info.sType = 
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    
    pipeline_create_info.pStages = program.getStageInfo(
        &pipeline_create_info.stageCount
    );
    
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly;

    pipeline_create_info.pDynamicState = &dynamic_state;
    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pColorBlendState = &color_blending;
    pipeline_create_info.pRasterizationState = program.getRasterizerInfo();
    pipeline_create_info.pMultisampleState = program.getMultisamplingInfo();
    pipeline_create_info.pDepthStencilState = program.getDepthStencilInfo();

    // TODO: tessellation state
    pipeline_create_info.pTessellationState = VK_NULL_HANDLE;

    pipeline_create_info.layout = out_pipeline.m_layout;

    pipeline_create_info.renderPass = render_pass.m_handle;
    pipeline_create_info.subpass = 0;
    pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_create_info.basePipelineIndex = -1;

    VkResult res_pipeline = vkCreateGraphicsPipelines(
        logical, 
        VK_NULL_HANDLE, 
        1, &pipeline_create_info, 
        m_allocator, 
        &out_pipeline.m_handle
    );

    if (res_pipeline != VK_SUCCESS) {
        vkDestroyPipelineLayout(
            logical, 
            out_pipeline.m_layout, 
            m_allocator
        );
        
        throw PipelineCreationError(
            "Pipeline creation error: {}",
            vk_error_str(res_pipeline)
        );
    }
    
    return out_pipeline;
}

// Destroy a vulkan graphic pipeline
void Device::destroyGraphicsPipeline(
    GraphicsPipeline &pipeline
) {
    // Destroy the pipeline data fields
    vkDestroyPipeline(
        logical, 
        pipeline.m_handle, 
        m_allocator
    );
    
    vkDestroyPipelineLayout(
        logical, 
        pipeline.m_layout, 
        m_allocator
    );

    pipeline = GraphicsPipeline();
}

// PRIVATE:
// Create the command pool 
CommandPool Device::createCmdPool(
    u32 queue_family_index,
    VkCommandPoolCreateFlags flags
) {
    CommandPool out_pool;
    out_pool.m_device_p = this;
    
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = flags;

    pool_info.queueFamilyIndex = queue_family_index;
    
    VkResult res = vkCreateCommandPool(
        logical,
        &pool_info,
        m_allocator, 
        &out_pool.m_handle
    );
    
    if (res != VK_SUCCESS) {
        throw CommandPoolInitError(
            "vkCreateCommandPool (queue family: {}) error: {}",
            queue_family_index,
            vk_error_str(res)
        );
    }

    return out_pool;
}

/*
 *
 *      Physical device functions
 *
 * */

// Pick a physical device that meet the requirement
void Device::pickPhysicalDevice(
    Context *context_p,
    PhysicalDeviceRequirement requirement
) {
    // Get device number
    u32 device_count = 0;
    vk_check(vkEnumeratePhysicalDevices(
        context_p->instance, 
        &device_count, 
        VK_NULL_HANDLE
    ));

    if (device_count == 0) {
        throw PhysicalDeviceError("No vulkan physical device found");
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vk_check(vkEnumeratePhysicalDevices(
        context_p->instance,
        &device_count,
        devices.data()
    ));
    
    // Find the best device
    u32 best_score = 0;
    u32 best_device = (u32)-1;

    for (u32 i = 0; i < device_count; i++) {
        // Check if the device fit the requirement
        bool device_suitable = checkDeviceRequirement(
            context_p,
            devices[i],
            requirement
        );

        // If the device is suitable rate it
        if (device_suitable) {
            u32 score = rateDevice(devices[i]);

            if (score > best_score) {
                best_device = i;
                best_score = score;
            }
        }
    }

    if (best_device == (u32)-1) {
        throw PhysicalDeviceError("No suitable vulkan physical device found");
    }

    // Store the selected physical device
    physical = devices[best_device];

    m_device_requirement = requirement;
    vkGetPhysicalDeviceProperties(physical, &m_physical_properties);

    // Print debug information
    printPhysicalDeviceInfo(
        devices[best_device],
        getQueueIndices(context_p, devices[best_device])
    );
}

// Return true if the physical device meet the given requirement
bool Device::checkDeviceRequirement(
    Context *context_p,
    
    VkPhysicalDevice device,
    PhysicalDeviceRequirement requirement
) {
    // Get device propriety
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);
    
    // Get queue indices
    QueueFamilyIndices indices = getQueueIndices(context_p, device);
    
    // Check if the device support all the required features
    bool support_features = checkDeviceFeatures(
        requirement.required_feature,
        device_features
    ); 
    
    if (!support_features)
        return false;
    
    // Check if the device support all the required queues
    bool support_queue = checkDeviceQueue(
        requirement.required_queue,
        indices.supportedQueue()
    );
    
    if (!support_queue)
        return false;

    // Check if the device support the required device extensions
    bool support_extensions =
        requirement.required_device_extensions.checkExtensions(device);   
    
    if (!support_extensions)
        return false;

    return true;
}

// Check if the device support the required features
bool Device::checkDeviceFeatures(
    VkPhysicalDeviceFeatures required_features,
    VkPhysicalDeviceFeatures available_features
) {
    STATIC_ASSERT(sizeof(VkBool32) == 4);
    STATIC_ASSERT(sizeof(VkPhysicalDeviceFeatures) % sizeof(VkBool32) == 0);
    
    #define FEATURE_COUNT sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)
    
    // Treat the VkBool32 struct as a VkBool32 array
    VkBool32 *required = (VkBool32*)&required_features;
    VkBool32 *available = (VkBool32*)&available_features;
    
    // Check if all the required features are available
    for (u32 i = 0; i < FEATURE_COUNT; i++) {
        if (required[i] == VK_TRUE && available[i] != VK_TRUE) {
            return false;
        }
    }
    
    return true;
}

// Check if the device support the required queue family
bool Device::checkDeviceQueue(
    QueueFamilyType required_queue,
    QueueFamilyType available_queue
) {
    if (required_queue.graphics() && !available_queue.graphics())
        return false;
    if (required_queue.compute() && !available_queue.compute())
        return false;
    if (required_queue.transfer() && !available_queue.transfer())
        return false;
    if (required_queue.present() && !available_queue.present())
        return false;

    return true;
}

// Return a score for the given physical device, higher is better
u32 Device::rateDevice(VkPhysicalDevice device)
{
    // Get device propriety
    VkPhysicalDeviceProperties device_prop;
    vkGetPhysicalDeviceProperties(device, &device_prop);
    
    // Rate the device
    u32 device_score = 1;
    
    if (device_prop.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        device_score += 2000;
    }

    // More memory, better
    device_score += device_prop.limits.maxImageDimension2D;
    
    return device_score;
}

// Get the device queue family indices
Device::QueueFamilyIndices Device::getQueueIndices(
    Context *context_p,
    VkPhysicalDevice device
) {
    u32 graphics_family = (u32)-1;
    u32 compute_family  = (u32)-1;
    u32 transfer_family = (u32)-1;
    u32 present_family  = (u32)-1;

    // Get queue family count
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queue_family_count, NULL
    );

    // Get queue family proprieties
    std::vector<VkQueueFamilyProperties> queues_prop(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, 
        &queue_family_count,
        queues_prop.data()
    );

    // Find queue family
    u32 dedicated_transfer_queue = (u32)-1;
    u32 dedicated_compute_queue = (u32)-1;
    u32 graphics_and_present_queue = (u32)-1;
    
    for (u32 i = 0; i < queue_family_count; i++) {
        bool graphic_support  = false;
        bool compute_support  = false;
        bool transfer_support = false;
        bool present_support  = false;

        VkBool32 present_vk_bool = VK_FALSE;
        vk_check(vkGetPhysicalDeviceSurfaceSupportKHR(
            device, 
            i, 
            context_p->surface, 
            &present_vk_bool
        ));
        
        // Check for queue family support
        VkQueueFlags queue_flags = queues_prop[i].queueFlags;
        
        graphic_support  = (queue_flags & VK_QUEUE_GRAPHICS_BIT) != 0;
        compute_support  = (queue_flags & VK_QUEUE_COMPUTE_BIT) != 0 ;
        transfer_support = (queue_flags & VK_QUEUE_TRANSFER_BIT) != 0 ;
        present_support  = present_vk_bool == VK_TRUE;
        
        // Store indices
        if (graphic_support)
            graphics_family = i;
        if (compute_support)
            compute_family = i;
        if (transfer_support)
            transfer_family = i;
        if (present_support)
            present_family = i;
        
        // Look for dedicated compute queue
        if (compute_support && !graphic_support)
            dedicated_compute_queue = i;
        
        // Look for dedicated transfer queue
        if (transfer_support && (!compute_support && !graphic_support))
            dedicated_transfer_queue = i;

        // Look for a queue with both graphics and present functionality
        if (graphic_support && present_support)
            graphics_and_present_queue = i;
    }

    // Replace the stored queue with the dedicated one if they were found
    if (dedicated_compute_queue != (u32)-1) {
        compute_family = dedicated_compute_queue;
    }
    
    if (dedicated_transfer_queue != (u32)-1) {
        transfer_family = dedicated_transfer_queue;
    }

    // Use the same queue for graphics and present operation
    // this allow the swap image to use exclusive share mode
    if (graphics_and_present_queue != (u32)-1) {
        graphics_family = graphics_and_present_queue;
        present_family  = graphics_and_present_queue;
    }

    // Store the supported queue family struct
    bool graphic_support  = graphics_family != (u32)-1;  
    bool compute_support  = compute_family != (u32)-1;  
    bool transfer_support = transfer_family != (u32)-1;  
    bool present_support  = present_family != (u32)-1;  
    
    QueueFamilyType supported_queue(
        graphic_support,
        compute_support,
        transfer_support,
        present_support
    );

    // Build and return the indices
    QueueFamilyIndices indices(
        supported_queue,
        
        graphics_family,
        compute_family,
        transfer_family,
        present_family
    );

    return indices;
}

// Print debug information for the given physical device
void Device::printPhysicalDeviceInfo(
    VkPhysicalDevice device,
    QueueFamilyIndices indices
) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(device, &device_properties);

    // Print device name
    log::core::trace(
        "Vulkan physical device name: {}",
        device_properties.deviceName
    );

    // Print vulkan version
    u32 major = VK_VERSION_MAJOR(device_properties.apiVersion);
    u32 minor = VK_VERSION_MINOR(device_properties.apiVersion);
    u32 patch = VK_VERSION_PATCH(device_properties.apiVersion);

    log::core::trace("Vulkan API version: {}.{}.{}", major, minor, patch);
    
    // Print device queue family indices
    log::core::trace(
        "Vulkan device graphics queue family index: {}",
        indices.graphicsIndex()
    );
    log::core::trace(
        "Vulkan device compute queue family index: {}",
        indices.computeIndex()
    );
    log::core::trace(
        "Vulkan device transfer queue family index: {}",
        indices.transferIndex()
    );
    log::core::trace(
        "Vulkan device present queue family index: {}",
        indices.presentIndex()
    );
}

/*
 *
 *      Extensions functions
 *
 * */

// Return a vector containing the device extensions 
const std::vector<const char*> Device::Extensions::getExtensions()
{
    usize ext_count = m_extensions.size();
    if (m_swap_chain)
        ext_count += 1;

    std::vector<const char*> out_extensions;
    out_extensions.reserve(ext_count);

    // If the swap chain extensions is required add it to the vector
    out_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    // Add all the extensions provided in the constructor to the output 
    for (auto ext : m_extensions) {
        out_extensions.push_back(ext);
    }

    return out_extensions;
}

// Return true if all the extensions store in the object
// are supported by the given physical device
bool Device::Extensions::checkExtensions(VkPhysicalDevice device)
{
    std::vector<const char*> extensions = getExtensions();
    
    u32 extension_count;
    vk_check(vkEnumerateDeviceExtensionProperties(
        device,
        VK_NULL_HANDLE, 
        &extension_count, 
        VK_NULL_HANDLE
    ));

    // Store the proprieties in a vector
    std::vector<VkExtensionProperties> available_extensions(extension_count);
    vk_check(vkEnumerateDeviceExtensionProperties(
        device, 
        VK_NULL_HANDLE,
        &extension_count, 
        available_extensions.data()
    ));

    // Check if all the required extensions are supported
    for (usize i = 0; i < extensions.size(); i++) {
        bool extension_found = false;

        for (usize j = 0; i < extension_count; j++) {
            std::string_view required_ext = extensions[i];
            std::string_view available_ext = 
                available_extensions[j].extensionName;

            // Compare the two strings and check for extension support
            if (required_ext.compare(available_ext) == 0) {
                extension_found = true;
                break;
            }
        }

        // Return false if the extension was not found
        if (!extension_found) {
            return false;
        }
    }

    return true;
}

} // namespace cndt::vulkan
