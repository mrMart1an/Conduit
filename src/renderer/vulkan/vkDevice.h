#ifndef CNDT_VK_DEVICE_H
#define CNDT_VK_DEVICE_H

#include "conduit/defines.h"

#include "renderer/vulkan/deleteQueue.h"
#include "renderer/vulkan/initialization/vkContext.h"
#include "renderer/vulkan/storage/vkBuffer.h"
#include "renderer/vulkan/storage/vkImage.h"

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

    // Bind the given Image to the device with the given memory offset
    void bind(Image &image, VkDeviceSize memory_offset = 0);

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

    // Bind the given buffer to the device with the given memory offset
    void bind(Buffer &buffer, VkDeviceSize memory_offset = 0);
    
    // Map the buffer to a region of host memory and return a pointer to it
    void* mapBuffer(
        Buffer &buffer,
        
        VkDeviceSize offset,
        VkDeviceSize size,

        VkMemoryMapFlags map_flags
    );

    // Unmap the given buffer 
    void unmapBuffer(Buffer &buffer);
    
private:
    /*
     *
     *      Memory functions
     *
     * */
    
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
    // add the pool to the delete queue
    VkCommandPool createCmdPool(
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

    // Device memory property for allocation
    VkPhysicalDeviceMemoryProperties memory_properties;
    
    VkQueue graphics_queue;
    VkQueue compute_queue;
    VkQueue transfer_queue;
    VkQueue present_queue;
    
    // Command pool for graphics operations
    VkCommandPool graphics_cmd_pool;
    // Command pool for transient transfer commands
    VkCommandPool transfer_transient_cmd_pool;
    
private:
    // Store custom allocator callbacks
    const VkAllocationCallbacks *m_allocator;

    // Queue family indices 
    QueueFamilyIndices m_queue_indices;

    // Physical device requirement
    PhysicalDeviceRequirement m_device_requirement;

    // Device delete queue
    DeleteQueue m_delete_queue;
};

} // namespace cndt::vulkan

#endif
