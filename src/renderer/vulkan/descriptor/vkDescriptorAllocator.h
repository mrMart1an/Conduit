#ifndef CNDT_VK_DESCRIPTOR_ALLOCATOR_H
#define CNDT_VK_DESCRIPTOR_ALLOCATOR_H

#include "conduit/defines.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

// Descriptor set dynamic allocator
class DescriptorAllocator {
    friend class Device;
    
    static constexpr u32 m_staring_set_per_pool = 20;

public:
    struct PoolSizeRatio {
        PoolSizeRatio(VkDescriptorType type, u32 ratio) : 
            type(type), ratio(ratio) { }

        VkDescriptorType type;
        u32 ratio;
    };
    
public:
    DescriptorAllocator() = default;
    
    // Allocate a descriptor set
    VkDescriptorSet allocate(VkDescriptorSetLayout layout);
    
    // Clear all the descriptor set allocated from the pool
	void clearPools();

private:
    // Get new pool from the free pools list or create a new one
    VkDescriptorPool getFreePool();

    // Create a new command pool
    VkDescriptorPool createPool();

    // Destroy all the pools in the allocator
    void destroyPools();

private:
    std::vector<VkDescriptorPool> m_full_pools;
    std::vector<VkDescriptorPool> m_free_pools;

    VkDescriptorPool m_in_use_pool;
    
    // Store the ratio of descriptor binding type 
    // for each set allocated from the pool
    std::vector<PoolSizeRatio> m_ratios;
    
    // Descriptor set for new pools
    u32 m_set_per_pool;

    // Device that own the allocator
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
