#ifndef CNDT_VK_DEVICE_H
#define CNDT_VK_DEVICE_H

#include "conduit/defines.h"
#include "conduit/internal/core/deleteQueue.h"

#include "renderer/vulkan/descriptor/vkDescriptorLayout.h"
#include "renderer/vulkan/pipelines/vkPipeline.h"
#include "renderer/vulkan/pipelines/vkShaderModule.h"
#include "renderer/vulkan/storage/vkBuffer.h"
#include "renderer/vulkan/storage/vkGeometryBuffer.h"
#include "renderer/vulkan/storage/vkImage.h"

#include "renderer/vulkan/sync/vkFence.h"
#include "renderer/vulkan/vkCommandBuffer.h"
#include "renderer/vulkan/vkCommandPool.h"
#include "renderer/vulkan/vkRenderAttachment.h"
#include "renderer/vulkan/vkRenderPass.h"
#include "renderer/vulkan/vkContext.h"

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace cndt::vulkan {

// Vulkan device class
class Device {
public:
    // Store the device extensions
    class Extensions {
    public:
        Extensions(
            bool swap_chain_extension = false,
            std::vector<const char*> extensions = std::vector<const char*>()
        ) : m_extensions(extensions),
            m_swap_chain(swap_chain_extension) 
        { };
        
        // Return a vector containing the device extensions 
        const std::vector<const char*> getExtensions();

        // Return true if all the extensions store in the object
        // are supported by the given physical device
        bool checkExtensions(VkPhysicalDevice physical_device);
        
    private:
        // Store the extensions provided in the constructor 
        std::vector<const char*> m_extensions;

        // If true the swap chain extension is enabled
        bool m_swap_chain;
    };

    // Store the queue family type
    class QueueFamilyType {
    public:
        QueueFamilyType() = default;
        
        QueueFamilyType(
            bool graphics,
            bool compute,
            bool transfer,
            bool present
        ) : m_graphics(graphics),
            m_compute(compute),
            m_transfer(transfer),
            m_present(present) 
        { }

        // Return true if the queue family support graphics operations
        bool graphics() const { return m_graphics; };
        // Return true if the queue family support compute operations
        bool compute() const { return m_compute; };
        // Return true if the queue family support transfer operations
        bool transfer() const { return m_transfer; };
        // Return true if the queue family support present operations
        bool present() const { return m_present; };
        
    private:
        bool m_graphics;
        bool m_compute;
        bool m_transfer;
        bool m_present;
    };

    // Store the queue type
    enum class QueueType {
        Graphics,
        Compute,
        Transfer
    };
    
    // Store the device queue family indices
    class QueueFamilyIndices {
    public:
        QueueFamilyIndices() = default;
        
        QueueFamilyIndices(
            QueueFamilyType supported_queue,
            
            u32 graphics_index,
            u32 compute_index,
            u32 transfer_index,
            u32 present_index
        ) : m_supported_queue(supported_queue),
            
            m_indices{
                graphics_index,
                compute_index,
                transfer_index,
                present_index
                }
        { };

        // Graphics queue index
        u32 graphicsIndex() const 
        { 
            return (m_supported_queue.graphics() ? m_indices[0] : (u32)-1);
        };
        // Compute queue index
        u32 computeIndex() const
        { 
            return (m_supported_queue.compute() ? m_indices[1] : (u32)-1);
        };
        // Transfer queue index
        u32 transferIndex() const
        { 
            return (m_supported_queue.transfer() ? m_indices[2] : (u32)-1);
        };
        // Present queue index
        u32 presentIndex() const
        { 
            return (m_supported_queue.present() ? m_indices[3] : (u32)-1);
        };

        // indices array [graphics, compute, transfer, present]
        const u32* indices() const { return m_indices; };

        // Return the supported queue 
        QueueFamilyType supportedQueue() const { return m_supported_queue; };
        
    private:
        // Store the supported queue type by the device
        QueueFamilyType m_supported_queue;
        
        // Store the indices of the queue family
        u32 m_indices[4];
    };
    
    // Store the physical device requirement 
    struct PhysicalDeviceRequirement {
        Extensions required_device_extensions;
        
        VkPhysicalDeviceFeatures required_feature;
        QueueFamilyType required_queue;
    };

public:
    // Initialize a vulkan device
    void initialize(
        Context *context_p,
        PhysicalDeviceRequirement physical_device_requirement
    );

    // Shutdown vulkan device
    void shutdown();

    /*
     *
     *      Command pool functions
     *
     * */
    
    // Create the command pool  
    // add the pool to the delete queue
    CommandPool createCmdPool(
        QueueType queue_type,
        VkCommandPoolCreateFlags flags = 0
    );

    // Destroy the given command pool
    void destroyCmdPool(CommandPool cmd_pool);

    /*
     *
     *      Render pass functions
     *
     * */

    // Create a vulkan render pass
    RenderPass createRenderPass(
        VkFormat attachment_format,
        RenderPass::ClearColor clear_color
    );

    // Destroy render pass
    void destroyRenderPass(RenderPass &render_pass);

    /*
     *
     *      Render attachment functions
     *
     * */
    
    // Create a vulkan render attachment
    RenderAttachment createRenderAttachment(
        RenderPass render_pass,
        
        VkImageView image_view,
        VkExtent2D image_extent,
        VkFormat image_format
    );
    
    // Create a vulkan render attachment
    RenderAttachment createRenderAttachment(
        RenderPass render_pass,
        Image &image
    );

    // Destroy the given vulkan render attachment
    void destroyRenderAttachment(RenderAttachment &attachment);

    /*
     *
     *      Sync object functions
     *
     * */

    // Create a fence, used for GPU to CPU synchronization 
    // If the signaled argument is true the fence is signaled on creation
    Fence createFence(bool signaled);

    // Destroy the given fence
    void destroyFence(Fence &fence);

    // Create a semaphore, used for GPU to GPU synchronization
    VkSemaphore createSemaphore();

    // Destroy the given semaphore
    void destroySemaphore(VkSemaphore &semaphore);

    
    /*
     *
     *      Image functions
     *
     * */

    // Create a new image with the given requirement 
    Image createImage(
        u32 width, 
        u32 height, 
        
        VkFormat image_format,
        bool linear_tiling,

        VkImageUsageFlagBits usage_bits,
        VkMemoryPropertyFlags memory_property,

        bool bind_on_create
    );

    // Destroy the given image
    void destroyImage(Image &image);

    /*
     *
     *     Buffer functions
     *
     * */

    // Create a new buffer with the given requirement
    Buffer createBuffer(
        VkDeviceSize size,
        
        VkBufferUsageFlagBits usage_bits,
        VkMemoryPropertyFlags memory_flags,
        
        bool bind_on_create
    );

    // Destroy the given buffer
    void destroyBuffer(Buffer &buffer);
    
    // Resize the given buffer
    // this operation is blocking
    void bufferResize(
        Buffer &buffer,
        VkDeviceSize size
    );

    // Copy the content of one buffer to another
    // this operation is blocking
    void copyBuffer(
        VkDeviceSize src_offset,
        Buffer &src_buffer,
        
        VkDeviceSize dest_offset,
        Buffer &dest_buffer,

        VkDeviceSize size
    );

    // Copy the content of one buffer to another
    // this operation is blocking
    void copyBuffer(
        VkDeviceSize src_offset,
        VkBuffer src_buffer,
        
        VkDeviceSize dest_offset,
        VkBuffer dest_buffer,

        VkDeviceSize size
    );

    /*
     *
     *      Geometry buffer functions
     *
     * */

    // Initialize the vertex and index buffer in the geometry buffer object
    template<typename VertexType>
    GeometryBuffer<VertexType> createGeometryBuffer(
        u64 vertex_count,
        u64 index_count
    );
    
    // Destroy the vertex and index buffers in the geometry buffer object
    template<typename VertexType>
    void destroyGeometryBuffer(
        GeometryBuffer<VertexType> &buffer
    );
    
    // Load mesh data in the geometry buffer
    template<typename VertexType>
    void geometryBufferLoad(
        GeometryBuffer<VertexType> &buffer,
        
        std::vector<VertexType> vertices,
        std::vector<u32> indices
    );
    
    /*
     *
     *      Shader module functions
     *
     * */

    // Create a shader module for the required stage from the SPIR-V file
    ShaderModule createShaderModule(
        const char* filepath,
        VkShaderStageFlagBits stage_flag_bits
    );

    // Destroy a shader module
    void destroyShaderModule(
        ShaderModule &module
    );
        
    /*
     *
     *      Pipeline functions
     *
     * */

    // Create a vulkan graphics pipeline
    GraphicsPipeline createGraphicsPipeline(
    	RenderPass &render_pass,

    	std::string vertex_shader_filepath,
    	std::string fragment_shader_filepath,
	
	    std::vector<VkDescriptorSetLayout> descriptor_set_layout,
    	
    	bool wireframe = false
    );
    
    // Destroy a vulkan graphic pipeline
    void destroyGraphicsPipeline(
    	GraphicsPipeline &pipeline
    );

    /*
     *
     *      Descriptor set functions
     *
     * */

    // Create the descriptor layout builder
    DescriptorLayoutBuilder createDescriptorLayoutBuilder();
    
    /*
     *
     *      Getter
     *
     * */

    // Return the device queue family indices
    QueueFamilyIndices queueIndices() const { return m_queue_indices; }
    
    // Get the allocator callbacks pointer
    const VkAllocationCallbacks *allocator() const { return m_allocator; }

private:
    
    /*
     *
     *      Memory functions
     *
     * */
        
    // Allocate device memory with the required property
    VkDeviceMemory allocateMemory(
        VkMemoryRequirements requirements,
        VkMemoryPropertyFlags memory_flags
    );
    
    // Free the given device memory 
    void freeMemory(VkDeviceMemory memory);
    
    // Find the index of a suitable memory type
    u32 findMemoryTypeIndex(
        u32 type_bits,
        VkMemoryPropertyFlags properties
    );
    
    /*
     *
     *      Queue functions
     *
     * */

    // Retrieve the queue from the logical device
    void retrieveQueue();

    /*
     *
     *      Command pool functions
     *
     * */
    
    // Create the command pool  
    CommandPool createCmdPool(
        u32 queue_family_index,
        VkCommandPoolCreateFlags flags
    );

    /*
     *
     *      Logical device functions
     *
     * */

    // Create the logical device
    void createLogicalDevice(Context *context_p);
    
    // Destroy the logical device
    void destroyLogicalDevice();
    
    /*
     *
     *      Physical device functions
     *
     * */

    // Pick a physical device that meet the requirement
    void pickPhysicalDevice(
        Context *context_p,
        PhysicalDeviceRequirement requirement
    );

    // Return true if the physical device meet the given requirement
    bool checkDeviceRequirement(
        Context *context_p,
        
        VkPhysicalDevice device,
        PhysicalDeviceRequirement requirement
    );
    // Check if the device support the required features
    bool checkDeviceFeatures(
        VkPhysicalDeviceFeatures required_feature,
        VkPhysicalDeviceFeatures available_feature
    );
    // Check if the device support the required queue family
    bool checkDeviceQueue(
        QueueFamilyType required_queue,
        QueueFamilyType available_queue
    );
    
    // Return a score for the given physical device, higher is better
    u32 rateDevice(VkPhysicalDevice device);

    // Print debug information for the given physical device
    void printPhysicalDeviceInfo(
        VkPhysicalDevice device, 
        QueueFamilyIndices indices
    );

    // Get the device queue family indices
    QueueFamilyIndices getQueueIndices(
        Context *context_p,
        VkPhysicalDevice device
    );

public:
    // Logical vulkan device
    VkDevice logical;

    // Physical vulkan device
    VkPhysicalDevice physical;

    VkQueue graphics_queue;
    VkQueue compute_queue;
    VkQueue transfer_queue;
    VkQueue present_queue;
    
private:
    // Store custom allocator callbacks
    const VkAllocationCallbacks *m_allocator;

    // Queue family indices 
    QueueFamilyIndices m_queue_indices;

    // Physical device requirement
    PhysicalDeviceRequirement m_device_requirement;
    
    // Device memory property for allocation
    VkPhysicalDeviceMemoryProperties m_memory_properties;

    // Transfer operation command pool
    CommandPool m_transfer_cmd_pool;
    // Transfer operation fence
    Fence m_transfer_fence;
    
    // Device delete queue
    DeleteQueue m_delete_queue;
};


/*
 *
 *      Template implementation
 *
 * */

// Initialize the vertex and index buffer in the geometry buffer object
template<typename VertexType>
GeometryBuffer<VertexType> Device::createGeometryBuffer(
    u64 vertex_count,
    u64 index_count
) {
    GeometryBuffer<VertexType> out_buffer;

    VkMemoryPropertyFlags mem_prop = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    VkBufferUsageFlagBits vertex_usage = static_cast<VkBufferUsageFlagBits>(
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );
    VkBufferUsageFlagBits index_usage = static_cast<VkBufferUsageFlagBits>(
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT | 
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT
    );

    // Create the vertex buffer
    out_buffer.m_vertex_buffer = createBuffer(
        vertex_count * sizeof(VertexType), 
        vertex_usage,
        mem_prop, 
        true
    );

    // Create the index buffer
    out_buffer.m_index_buffer = createBuffer(
        index_count * sizeof(u32), 
        index_usage,
        mem_prop, 
        true
    );
    
    // Set load offsets to 0
    out_buffer.m_vertex_load_offset = 0;
    out_buffer.m_index_load_offset = 0;

    return out_buffer;
}

// Destroy the vertex and index buffers in the geometry buffer object
template<typename VertexType>
void Device::destroyGeometryBuffer(
    GeometryBuffer<VertexType> &buffer
) {
    destroyBuffer(buffer.m_vertex_buffer);
    destroyBuffer(buffer.m_index_buffer);
}

// Load mesh data in the geometry buffer
template<typename VertexType>
void Device::geometryBufferLoad(
    GeometryBuffer<VertexType> &buffer,
    
    std::vector<VertexType> vertices,
    std::vector<u32> indices
) {
    VkDeviceSize vertex_data_size = vertices.size() * sizeof(VertexType);
    VkDeviceSize index_data_size = indices.size() * sizeof(u32);

    // Create the staging buffers
    VkMemoryPropertyFlags staging_mem_prop =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    
    Buffer vertex_staging = createBuffer(
        vertex_data_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        staging_mem_prop,
        true
    );
    Buffer index_staging = createBuffer(
        index_data_size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        staging_mem_prop,
        true
    );

    // Load the staging buffer with the data
    vertex_staging.loadBuffer(
        0, 
        0, vertex_data_size,
        vertices.data()
    );
    index_staging.loadBuffer(
        0, 
        0, index_data_size,
        indices.data()
    );

    // Copy the staging buffers to the geometry buffer
    copyBuffer(
        0, vertex_staging,
        buffer.m_vertex_load_offset,
        buffer.m_vertex_buffer,
        vertex_data_size
    );
    copyBuffer(
        0, index_staging,
        buffer.m_index_load_offset,
        buffer.m_index_buffer,
        index_data_size
    );
    
    // Increment the loading offset
    buffer.m_vertex_load_offset += vertex_data_size;
    buffer.m_index_load_offset += index_data_size;

    // Destroy the staging buffers
    destroyBuffer(vertex_staging);
    destroyBuffer(index_staging);
}

} // namespace cndt::vulkan

#endif
