#include "conduit/defines.h"
#include "conduit/logging.h"
#include "conduit/internal/core/deleteQueue.h"

#include "conduit/renderer/vertex.h"
#include "renderer/vulkan/descriptor/vkDescriptorLayout.h"
#include "renderer/vulkan/utils/vkAttributeDescriptor.h"
#include "renderer/vulkan/utils/vkValidation.h"
#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkDevice.h"
#include "renderer/vulkan/storage/vkImage.h"
#include "renderer/vulkan/vkCommandPool.h"

#include <array>
#include <iostream>
#include <fstream>
#include <cstring>
#include <format>
#include <functional>
#include <string_view>
#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Initialize a vulkan device
void Device::initialize(
    Context *context_p,
    PhysicalDeviceRequirement physical_device_requirement
) {
    log::core::debug("Creating vulkan device");

    m_allocator = context_p->allocator;

    // Pick a suitable physical device matching the requirement
    pickPhysicalDevice(context_p, physical_device_requirement);

    // Create the logical device
    createLogicalDevice(context_p);
    m_delete_queue.addDeleter(std::bind(&Device::destroyLogicalDevice, this));

    // Retrieve the device queue
    retrieveQueue();

    // Create transfer queue and fence
    if (m_device_requirement.required_queue.transfer()) {
        // Create the transient command pool for transfer operation
        m_transfer_cmd_pool = createCmdPool(
            m_queue_indices.transferIndex(),
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
        );

        m_delete_queue.addDeleter(std::bind(
            &Device::destroyCmdPool,
            this,
            std::ref(m_transfer_cmd_pool)
        ));

        // Create the transfer operation fence
        m_transfer_fence = createFence(false);

        m_delete_queue.addDeleter(std::bind(
            &Device::destroyFence,
            this,
            std::ref(m_transfer_fence)
        ));
    }
}

// Shutdown vulkan device
void Device::shutdown()
{
    log::core::debug("Destroying vulkan device");

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
        throw RenderPassCreationError(std::format(
            "Render pass creation error {}",
            vk_error_str(res)
        ));
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
        throw RenderAttachmentCreationError(std::format(
            "frame buffer creation error {}",
            vk_error_str(res)
        ));
    }

    return out_attachment;
}

// Create a vulkan render attachment
RenderAttachment Device::createRenderAttachment(
    RenderPass render_pass,
    Image &image
) {
    return createRenderAttachment(
        render_pass,
        
        image.m_view,
        image.m_image_extent,
        image.m_image_format
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
        throw FenceInitError(std::format(
            "Vulkan fence initialization error {}",
            vk_error_str(res)
        ));
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
        throw SemaphoreInitError(std::format(
            "Vulkan semaphore initialization error {}",
            vk_error_str(res)
        ));
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
Image Device::createImage(
    u32 width, 
    u32 height, 
    
    VkFormat image_format,
    bool linear_tiling,

    VkImageUsageFlagBits usage_bits,
    VkMemoryPropertyFlags memory_property,

    bool bind_on_create
) {
    Image out_image;
    out_image.m_device_p = this;
    
    out_image.m_image_format = image_format;
    out_image.m_usage_bits = usage_bits;
    out_image.m_memory_flags = memory_property;
    out_image.m_image_extent.width = width;
    out_image.m_image_extent.height = height;

    // Prepare the create info struct
    VkImageCreateInfo image_info = { };
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = image_format;
    image_info.tiling =
        linear_tiling ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = usage_bits;

    // TODO: Multi sampling
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    // TODO: Non exclusive sharing
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    // Create the image handle
    VkResult res = vkCreateImage(
        logical,
        &image_info,
        NULL,
        &out_image.m_handle
    );

    if (res != VK_SUCCESS) {
        throw ImageCreateError(std::format(
            "Image handle creation error: {}",
            vk_error_str(res)
        ));
    }

    // Allocate the image memory
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(
        logical,
        out_image.m_handle,
        &requirements
    );

    out_image.m_memory = allocateMemory(
        requirements,
        memory_property
    );
    
    // Create image view
    VkImageViewCreateInfo view_info = { };
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.image = out_image.m_handle;
    view_info.format = image_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VkResult view_res = vkCreateImageView(
        logical,
        &view_info,
        NULL,
        &out_image.m_view
    );

    if (view_res != VK_SUCCESS) {
        throw ImageCreateError(std::format(
            "image view creation error {}",
            vk_error_str(view_res)
        ));
    }
    
    // Bind the image if necessary
    if (bind_on_create) 
        out_image.bind();
    
    return out_image;
}

// Destroy the given image
void Device::destroyImage(Image &image)
{
    vkDestroyImageView(logical, image.m_view, m_allocator);
    vkDestroyImage(logical, image.m_handle, m_allocator);
    
    freeMemory(image.m_memory);

    image = Image();
}

/*
 *
 *     Buffer functions
 *
 * */

// Create a new buffer with the given requirement
Buffer Device::createBuffer(
    VkDeviceSize size,
    
    VkBufferUsageFlagBits usage_bits,
    VkMemoryPropertyFlags memory_flags,
    
    bool bind_on_create
) {
    Buffer out_buffer;
    out_buffer.m_device_p = this;
    
    out_buffer.m_usage_bits = usage_bits;
    out_buffer.m_memory_flags = memory_flags;
    
    out_buffer.m_size = size;
    out_buffer.m_mapped = false;

    // Create the buffer
    VkBufferCreateInfo buffer_info = { };
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage_bits;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult res = vkCreateBuffer(
        logical,
        &buffer_info, 
        m_allocator, 
        &out_buffer.m_handle
    );

    if (res != VK_SUCCESS) {
        throw BufferCreateError(std::format(
            "Vulkan buffer create error: {}",
            vk_error_str(res)
        ));
    }

    // Allocate the device memory
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(
        logical,
        out_buffer.m_handle,
        &requirements
    );

    out_buffer.m_memory = allocateMemory(
        requirements,
        out_buffer.m_memory_flags
    );

    if (bind_on_create) 
        out_buffer.bind();

    return out_buffer;
}

// Destroy the given buffer
void Device::destroyBuffer(Buffer &buffer) 
{
    if (buffer.m_handle != VK_NULL_HANDLE)
        vkDestroyBuffer(logical, buffer.m_handle, m_allocator);
    
    if (buffer.m_memory != VK_NULL_HANDLE)
        freeMemory(buffer.m_memory);

    buffer = Buffer();
}

// Resize the given buffer
// this operation is blocking
void Device::bufferResize(
    Buffer &buffer,
    VkDeviceSize size
) {
    VkBuffer new_buffer;
    VkDeviceMemory new_memory;

    // Create the buffer
    VkBufferCreateInfo buffer_info = { };
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = buffer.m_usage_bits;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult buf_res = vkCreateBuffer(
        logical,
        &buffer_info, 
        m_allocator, 
        &new_buffer
    );
    
    if (buf_res != VK_SUCCESS) {
        throw BufferCreateError(std::format(
            "Vulkan buffer resize error: {}",
            vk_error_str(buf_res)
        ));
    }
    
    // Allocate the device memory
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(
        logical,
        buffer.m_handle,
        &requirements
    );

    new_memory = allocateMemory(
        requirements,
        buffer.m_memory_flags
    );
    
    // Bind the buffer and copy the content of the old one in the new one
    VkResult bind_res = vkBindBufferMemory(logical, new_buffer, new_memory, 0);

    if (bind_res != VK_SUCCESS) {
        throw BufferBindError(std::format(
            "Vulkan buffer resize bind error: {}", 
            vk_error_str(bind_res)
        ));
    }
    
    // Copy the old buffer content in the new one
    copyBuffer(
        0, buffer.m_handle,
        0, new_buffer,
        buffer.m_size
    );

    // Destroy the old buffer memory and handle
    freeMemory(buffer.m_memory);   
    vkDestroyBuffer(logical, buffer.m_handle, m_allocator);

    // Assign the new buffer and memory
    buffer.m_handle = new_buffer;
    buffer.m_memory = new_memory;
    buffer.m_size = size;
}

// Copy the content of one buffer to another
// this operation is blocking
void Device::copyBuffer(
    VkDeviceSize src_offset,
    Buffer &src_buffer,
    
    VkDeviceSize dest_offset,
    Buffer &dest_buffer,

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
    // Allocate the command buffer
    CommandBuffer cmd_buffer = m_transfer_cmd_pool.allocateCmdBuffer();
    cmd_buffer.begin(true, false, false);
    
    VkBufferCopy copy_op = { };
    copy_op.size = size;
    copy_op.srcOffset = src_offset;
    copy_op.dstOffset = dest_offset;

    vkCmdCopyBuffer(
        cmd_buffer.handle(), 
        src_buffer, 
        dest_buffer, 
        1, &copy_op
    );

    cmd_buffer.end();
    cmd_buffer.submit(
        transfer_queue,
        m_transfer_fence.m_handle,
        0, VK_NULL_HANDLE,
        0, VK_NULL_HANDLE,
        0
    );

    // Wait for the operation to finish and free the command buffer
    m_transfer_fence.wait();
    
    m_transfer_cmd_pool.freeCmdBuffer(cmd_buffer);

    m_transfer_fence.reset();
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
    DescriptorAllocator out_allocator;
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

// PRIVATE ----------------------------

/*
 *
 *      Memory functions
 *
 * */

// Allocate device memory with the required property
VkDeviceMemory Device::allocateMemory(
    VkMemoryRequirements requirements,
    VkMemoryPropertyFlags memory_flags
) {
    // Find a suitable memory type
    u32 mem_type_index = findMemoryTypeIndex(
        requirements.memoryTypeBits, 
        memory_flags
    );
    
    VkMemoryAllocateInfo alloc_info = { };
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.memoryTypeIndex = mem_type_index;
    alloc_info.allocationSize = requirements.size;
    
    // Perform the allocation
    VkDeviceMemory out_memory;
    
    VkResult res = vkAllocateMemory(
        logical,
        &alloc_info,
        m_allocator,
        &out_memory
    );

    if (res != VK_SUCCESS) {
        throw DeviceMemoryError(std::format(
            "Device memory allocation error: {}",
            vk_error_str(res)
        ));
    } 
    
    return out_memory;
}

// Free the given device memory 
void Device::freeMemory(VkDeviceMemory memory)
{
    vkFreeMemory(logical, memory, m_allocator);
}

// Find the index of a suitable memory type
u32 Device::findMemoryTypeIndex(
    u32 type_bits,
    VkMemoryPropertyFlags flags
) {
    VkPhysicalDeviceMemoryProperties mem_props = m_memory_properties;
    
    for (u32 i = 0; i < mem_props.memoryTypeCount; i++) {
        bool type_bits_compatible = type_bits & (1 << i); 

        VkMemoryPropertyFlags available_flags =
            mem_props.memoryTypes[i].propertyFlags; 
        
        bool property_compatible = (available_flags & flags) == flags;  

        if (type_bits_compatible && property_compatible) {
            return i;
        }
    }

    // Throw an exception if no suitable type was found
    throw DeviceMemoryError("No suitable memory type for allocation");
}

/*
 *
 *      Logical device functions
 *
 * */

// Create the logical device
void Device::createLogicalDevice(Context *context_p)
{
    // Store device memory properties for allocations
    vkGetPhysicalDeviceMemoryProperties(
        physical,
        &m_memory_properties
    );

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
        throw DeviceInitError(std::format(
            "vkCreateDevice error: {}",
            vk_error_str(res)
        ));
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
ShaderModule Device::createShaderModule(
    const char* filepath,
    VkShaderStageFlagBits stage_flag_bits
) {
    ShaderModule out_shader;

    usize file_size;
    std::vector<char> file_buffer;

    // Read the file
    try {
        std::ifstream input_file;
        input_file.open(
            filepath, 
            std::ios::in | std::ios::binary | std::ios::ate
        );
        
        // Throw exception on failure
        if (input_file.fail()) {
            throw ShaderModuleFileError(std::format(
                "Vulkan shader module file access error, file: {}",
                filepath
            ));
        }

        // Store the file content in a vector
        file_size = input_file.tellg();
        input_file.seekg(0, std::ios::beg);
        
        file_buffer.resize(file_size);
        input_file.read(file_buffer.data(), file_size);
        
        input_file.close();
    } catch (const std::ifstream::failure& e) {
        throw ShaderModuleFileError(std::format(
            "Vulkan shader module file access error ({}), file: {}",
            e.what(),
            filepath
        ));
    }
    
    // Prepare the shader create info
    out_shader.m_create_info = { };
        
    out_shader.m_create_info.sType =
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    
    out_shader.m_create_info.codeSize = file_size;
    out_shader.m_create_info.pCode = (u32*)file_buffer.data();
    
    // Create the shader module
    VkResult res = vkCreateShaderModule(
        logical,
        &out_shader.m_create_info,
        m_allocator,
        &out_shader.m_handle
    );

    if (res != VK_SUCCESS) {
        throw ShaderModuleFileError(std::format(
            "Vulkan shader module creation error ({}), file: {}",
            vk_error_str(res),
            filepath
        ));
    }

    // Stage create info
    out_shader.m_shader_stage_create_info = { };
    
    out_shader.m_shader_stage_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    out_shader.m_shader_stage_create_info.stage = stage_flag_bits;
    out_shader.m_shader_stage_create_info.module = out_shader.m_handle;
    
    // Use main as default shader entry point
    out_shader.m_shader_stage_create_info.pName = "main"; 

    return out_shader;
}

// Destroy a shader module
void Device::destroyShaderModule(
    ShaderModule &module
) {
    vkDestroyShaderModule(logical, module.m_handle, m_allocator);
    module = ShaderModule();
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
    VkCommandPoolCreateFlags flags
) {
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

    return createCmdPool(index, flags);
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
 *      Pipeline functions
 *
 * */

// Create a vulkan graphics pipeline
GraphicsPipeline Device::createGraphicsPipeline(
	RenderPass &render_pass,

	std::string vertex_shader_filepath,
	std::string fragment_shader_filepath,

	std::vector<VkDescriptorSetLayout> descriptor_set_layout,
	
	bool wireframe 
) {
    GraphicsPipeline out_pipeline;

    // Load shader modules
    out_pipeline.m_vertex_stage = createShaderModule(
        vertex_shader_filepath.c_str(), 
		VK_SHADER_STAGE_VERTEX_BIT
    );

    // NOTE/TODO : ugly, handling this better
    try {
        out_pipeline.m_fragment_stage = createShaderModule(
            fragment_shader_filepath.c_str(), 
	    	VK_SHADER_STAGE_FRAGMENT_BIT
        );
    } catch (ShaderModuleException &e) {
        destroyShaderModule(out_pipeline.m_vertex_stage);
        throw e;
    }

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
    
    // Rasterizer create info
    VkPipelineRasterizationStateCreateInfo rasterizer = { };
    rasterizer.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;

    rasterizer.polygonMode = 
        wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;

    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // Multi sampling create info
    VkPipelineMultisampleStateCreateInfo multisampling = { };
    multisampling.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // TODO: configure depth and stencil 

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
    VkVertexInputBindingDescription binding_descriptor = 
        getBindingDescriptor<Vertex3D>();
    std::vector<VkVertexInputAttributeDescription> attributes = 
        getAttributeDescriptor<Vertex3D>();
    
    VkPipelineVertexInputStateCreateInfo vertex_input_info = { };
    vertex_input_info.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &binding_descriptor;
    vertex_input_info.vertexAttributeDescriptionCount = attributes.size();
    vertex_input_info.pVertexAttributeDescriptions = attributes.data();

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
        destroyShaderModule(out_pipeline.m_vertex_stage);
        destroyShaderModule(out_pipeline.m_fragment_stage);
        
        throw PipelineCreationError(std::format(
            "Pipeline layout creation error: {}",
            vk_error_str(res_layout)
        ));
    }
    
    // Create the pipeline info
    std::array<VkPipelineShaderStageCreateInfo, 2> stage_infos = {
        out_pipeline.m_vertex_stage.m_shader_stage_create_info,  
        out_pipeline.m_fragment_stage.m_shader_stage_create_info  
    };
    
    VkGraphicsPipelineCreateInfo pipeline_create_info = { };
    pipeline_create_info.sType = 
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.stageCount = stage_infos.size();
    pipeline_create_info.pStages = stage_infos.data();
    pipeline_create_info.pVertexInputState = &vertex_input_info;
    pipeline_create_info.pInputAssemblyState = &input_assembly;

    pipeline_create_info.pViewportState = &viewport_state;
    pipeline_create_info.pRasterizationState = &rasterizer;
    pipeline_create_info.pMultisampleState = &multisampling;
    pipeline_create_info.pColorBlendState = &color_blending;
    pipeline_create_info.pDynamicState = &dynamic_state;

    // TODO: depth state
    pipeline_create_info.pDepthStencilState = VK_NULL_HANDLE;
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
        
        destroyShaderModule(out_pipeline.m_vertex_stage);
        destroyShaderModule(out_pipeline.m_fragment_stage);
        
        throw PipelineCreationError(std::format(
            "Pipeline creation error: {}",
            vk_error_str(res_pipeline)
        ));
    }
    
    return out_pipeline;
}

// Destroy a vulkan graphic pipeline
void Device::destroyGraphicsPipeline(
    GraphicsPipeline &pipeline
) {
    destroyShaderModule(pipeline.m_vertex_stage);
    destroyShaderModule(pipeline.m_fragment_stage);

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
        throw CommandPoolInitError(std::format(
            "vkCreateCommandPool (queue family: {}) error: {}",
            queue_family_index,
            vk_error_str(res)
        ));
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
        log::core::trace(
            "Dedicated compute queue family found, index: {}",
            dedicated_compute_queue
        );
        
        compute_family = dedicated_compute_queue;
    }
    
    if (dedicated_transfer_queue != (u32)-1) {
        log::core::trace(
            "Dedicated transfer queue family found, index: {}",
            dedicated_transfer_queue
        );
        
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
