#ifndef CNDT_VK_DEVICE_H
#define CNDT_VK_DEVICE_H

#include "conduit/assets/handle.h"
#include "conduit/assets/shader.h"

#include "conduit/defines.h"
#include "conduit/internal/core/deleteQueue.h"

#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/buffer.h"
#include "conduit/renderer/image.h"

#include "renderer/vulkan/descriptor/vkDescriptorAllocator.h"
#include "renderer/vulkan/descriptor/vkDescriptorLayout.h"
#include "renderer/vulkan/descriptor/vkDescriptorWriter.h"
#include "renderer/vulkan/pipelines/vkPipeline.h"
#include "renderer/vulkan/pipelines/vkShaderModule.h"
#include "renderer/vulkan/pipelines/vkShaderProgram.h"
#include "renderer/vulkan/storage/vkBuffer.h"
#include "renderer/vulkan/storage/vkGeometryBuffer.h"
#include "renderer/vulkan/storage/vkImage.h"

#include "renderer/vulkan/sync/vkFence.h"
#include "renderer/vulkan/vkCommandBuffer.h"
#include "renderer/vulkan/vkCommandPool.h"
#include "renderer/vulkan/vkRenderPass.h"
#include "renderer/vulkan/vkContext.h"

#include <functional>
#include <vector>

#include <vk_mem_alloc.h>
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



    // Device queue abstraction
    class Queue {
    public:
        // Queue capability enum
        using CapabilityEnum = u32;
    
        struct Capability {
            Capability() = delete;
            ~Capability() = delete;
    
            static constexpr CapabilityEnum None = 0;
            static constexpr CapabilityEnum Graphics = CNDT_BIT(0);
            static constexpr CapabilityEnum Compute = CNDT_BIT(1);
            static constexpr CapabilityEnum Transfer = CNDT_BIT(2);
            static constexpr CapabilityEnum Present = CNDT_BIT(3);
        };

    public:
        Queue() = default;
        Queue(
            VkQueue handle,
            u32 index,

            CapabilityEnum capability
        ) : 
            m_handle(handle),
            m_family_index(index),
            m_capability(capability)
        { }

        // Get the queue handle
        VkQueue handle() const { return m_handle; }

        // Get the queue family index
        u32 familyIndex() const { return m_family_index; }
        // Get the queue capability
        CapabilityEnum capability() const { return m_capability; }
        
    private:
        VkQueue m_handle;
        u32 m_family_index;

        CapabilityEnum m_capability;
    };

    // Store the device queue family indices
    class QueueFamilyIndices {
    public:
        QueueFamilyIndices() = default;
        
        QueueFamilyIndices(
            std::pair<u32, Queue::CapabilityEnum> general,
            std::pair<u32, Queue::CapabilityEnum> compute,
            std::pair<u32, Queue::CapabilityEnum> transfer,
            std::pair<u32, Queue::CapabilityEnum> present
        ) : 
            m_indices
            {
                general.first,
                compute.first,
                transfer.first,
                present.first
            },
            m_capabilitys
            {
                general.second,
                compute.second,
                transfer.second,
                present.second
            }
        { };

        // Graphics queue index and capability
        std::pair<u32, Queue::CapabilityEnum> general() const 
        { 
            return { m_indices[0], m_capabilitys[0] };
        };
        // Compute queue index and capability
        std::pair<u32, Queue::CapabilityEnum> compute() const
        { 
            return { m_indices[1], m_capabilitys[1] };
        };
        // Transfer queue index and capability
        std::pair<u32, Queue::CapabilityEnum> transfer() const
        { 
            return { m_indices[2], m_capabilitys[2] };
        };
        // Present queue index and capability
        std::pair<u32, Queue::CapabilityEnum> present() const
        { 
            return { m_indices[3], m_capabilitys[3] };
        };

        // indices array [graphics, compute, transfer, present]
        const u32* indices() const { return m_indices; };

    private:
        // Store the indices of the queue family
        u32 m_indices[4];

        Queue::CapabilityEnum m_capabilitys[4];
    };
    
    // Store the physical device requirement 
    struct PhysicalDeviceRequirement {
        Extensions required_device_extensions;
        VkPhysicalDeviceFeatures required_feature;
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
        Queue queue,

        bool transient_pool = false,
        bool reset_cmd_buffer = false,
        bool protected_cmd_buffer = false
    );

    // Destroy the given command pool
    void destroyCmdPool(CommandPool cmd_pool);

    /*
     *
     *      Immediate command functions
     *
     * */

    // Execute the command in the given function and wait 
    // for them to complete on the CPU
    void runCmdImmediate(
        std::function<void(VkCommandBuffer)> immediate_fun
    );

    /*
     *
     *      Render pass functions
     *
     * */

    // Create a vulkan render pass
    RenderPass createRenderPass(const RenderPass::Info &info);

    // Destroy render pass
    void destroyRenderPass(RenderPass &render_pass);

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
    VulkanImage createImage(const GpuImage::Info& info);

    // Create swap chain image
    VulkanImage createSwapChainImage(
        VkImage image,
        
        GpuImage::Info::UsageEnum usage,
        GpuImage::Info::Format format,
        GpuImage::Extent extent
    );

    // Destroy the given image
    void destroyImage(VulkanImage &image);

    // Destroy swap chain image
    void destroySwapChainImage(VulkanImage &image);

    /*
     *
     *     Buffer functions
     *
     * */

    // Create a new buffer with the given requirement
    VulkanBuffer createBuffer(const GpuBuffer::Info& info);

    // Destroy the given buffer
    void destroyBuffer(VulkanBuffer &buffer);
    
    // Copy the content of one buffer to another
    // this operation is blocking
    void copyBuffer(
        VkDeviceSize src_offset,
        const VulkanBuffer &src_buffer,
        
        VkDeviceSize dest_offset,
        VulkanBuffer &dest_buffer,

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
    VulkanShaderModule createShaderModule(AssetHandle<Shader> shafer);

    // Destroy a shader module
    void destroyShaderModule(
        VulkanShaderModule &module
    );
     
    /*
     *
     *      Pipeline functions
     *
     * */

    // Create a vulkan graphics pipeline
    GraphicsPipeline createGraphicsPipeline(
    	RenderPass &render_pass,
        RenderRef<ShaderProgram> program,

	    std::vector<VkDescriptorSetLayout> descriptor_set_layout
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

    // Destroy the given descriptor set layout
    void destroyDescriptorLayout(DescriptorLayout &layout);

    // Create a descriptor set allocator
    DescriptorAllocator createDescriptorAllocator(
        std::vector<DescriptorAllocator::PoolSizeRatio> pool_ratio
    );

    // Destroy a descriptor allocator
    void destroyDescriptorAllocator(DescriptorAllocator &allocator);

    // Create a descriptor set writer
    DescriptorWriter createDescriptorWriter();
    
    /*
     *
     *      Getter
     *
     * */

    // Get the logical device handle
    VkDevice logical() const { return m_logical; }

    // Get the physical device handle
    VkPhysicalDevice physical() const { return m_physical; }

    // Return the device queue family indices
    QueueFamilyIndices queueIndices() const { return m_queue_indices; }

    // Get a const reference to the general purpose queue
    const Queue& generalQueue() const { return m_general_queue; }
    // Get a const reference to the compute queue
    const Queue& computeQueue() const { return m_compute_queue; }
    // Get a const reference to the transfer queue
    const Queue& transferQueue() const { return m_transfer_queue; }
    // Get a const reference to the present queue
    const Queue& presentQueue() const { return m_present_queue; }
    
    // Get the allocator callbacks pointer
    const VkAllocationCallbacks *allocator() const { return m_allocator; }

    // Get the vma allocator
    VmaAllocator vmaAllocator() const { return m_vma_allocator; } 

private:
   
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
     *      Vulkan memory allocator functions
     *
     * */

    // Initialize the device vma allocator
    void initializeVmaAllocator(VkInstance instance);

    // Shutdown the device vma allocator
    void shutdownVmaAllocator();

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
        QueueFamilyIndices available_queue
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

    /*
     *
     *      Conversion function (implementation in vkDeviceConverter.cpp)
     *
     * */

    // Convert backend agnostic buffer usage to vulkan buffer usage
    VkBufferUsageFlags getVkBufferUsage(GpuBuffer::Info::UsageEnum usage);

    // Convert backend agnostic image usage to vulkan image usage
    VkImageUsageFlags getVkImageUsage(GpuImage::Info::UsageEnum usage);

    // Convert backend agnostic image format to vulkan image format
    // TODO query for format support and fallback
    VkFormat getVkFormat(GpuImage::Info::Format format);

    // Convert the backend agnostic sample count to vulkan sample count
    // TODO check for sample count support
    VkSampleCountFlagBits getVkSampleCount(GpuImage::Info::Sample sample);

private:
    // Logical vulkan device
    VkDevice m_logical;

    // Physical vulkan device
    VkPhysicalDevice m_physical;

    // Queue family indices 
    QueueFamilyIndices m_queue_indices;

    // Physical device requirement
    PhysicalDeviceRequirement m_device_requirement;

    // Physical device property
    VkPhysicalDeviceProperties m_physical_properties;

    // Image next unique id
    VulkanImage::Id m_next_image_id;
    // Buffer next unique id
    VulkanImage::Id m_next_buffer_id;

    // Device queue
    Queue m_general_queue;
    Queue m_compute_queue;
    Queue m_transfer_queue;
    Queue m_present_queue;
    
    // Immediate command graphics command pool and buffer
    CommandPool m_general_cmd_pool;
    CommandBuffer m_general_cmd_buf;

    // Immediate command compute command pool and buffer
    CommandPool m_compute_cmd_pool;
    CommandBuffer m_compute_cmd_buf;

    // Immediate command transfer command pool and buffer
    CommandPool m_transfer_cmd_pool;
    CommandBuffer m_transfer_cmd_buf;

    // Immediate command execution fence
    Fence m_immediate_fence;
    
    // Store custom allocator callbacks
    const VkAllocationCallbacks *m_allocator;

    VmaAllocator m_vma_allocator;

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

    GpuBuffer::Info vertex_info = { };
    vertex_info.domain = GpuBuffer::Info::Domain::Device;
    vertex_info.size = vertex_count * sizeof(VertexType);
    vertex_info.usage = 
        GpuBuffer::Info::Usage::TransferDst |
        GpuBuffer::Info::Usage::VertexBuffer;

    GpuBuffer::Info index_info = { };
    index_info.domain = GpuBuffer::Info::Domain::Device;
    index_info.size = index_count * sizeof(u32);
    index_info.usage = 
        GpuBuffer::Info::Usage::TransferDst |
        GpuBuffer::Info::Usage::IndexBuffer;

    // Create the vertex buffer
    out_buffer.m_vertex_buffer = createBuffer(vertex_info);

    // Create the index buffer
    out_buffer.m_index_buffer = createBuffer(index_info);
    
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
    GpuBuffer::Info staging_info = {};
    staging_info.domain = GpuBuffer::Info::Domain::Host;
    staging_info.usage = GpuBuffer::Info::Usage::TransferSrc;
    
    staging_info.size = vertex_data_size;
    VulkanBuffer vertex_staging = createBuffer(staging_info);

    staging_info.size = index_data_size;
    VulkanBuffer index_staging = createBuffer(staging_info);

    // Load the staging buffer with the data
    vertex_staging.copyMemToBuf(
        vertices.data(),
        vertex_data_size, 0
    );
    index_staging.copyMemToBuf(
        indices.data(),
        index_data_size, 0
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
