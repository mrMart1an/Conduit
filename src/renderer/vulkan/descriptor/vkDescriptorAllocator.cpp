#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"
#include "renderer/vulkan/descriptor/vkDescriptorAllocator.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Allocate a descriptor set
VkDescriptorSet DescriptorAllocator::allocate(DescriptorLayout &layout)
{
    // If the m_in_use_pool is equal to null handle
    // Create a new pool and use it
    if (m_in_use_pool == VK_NULL_HANDLE) {
        m_in_use_pool = createPool();
    }

    VkDescriptorSet out_set;
    
    // Attempt pool allocation
    VkDescriptorSetAllocateInfo alloc_info = { };
    alloc_info.pNext = nullptr;
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = m_in_use_pool;
    alloc_info.descriptorSetCount = 1;
    
    VkDescriptorSetLayout set_layout = layout.layout();
    alloc_info.pSetLayouts = &set_layout;

    VkResult res = vkAllocateDescriptorSets(
        m_device_p->logical(),
        &alloc_info,
        &out_set
    );

    // If the allocation failed create a new pool and try again
    if (res == VK_ERROR_OUT_OF_POOL_MEMORY ||
        res == VK_ERROR_FRAGMENTED_POOL
    ) {

        m_full_pools.push_back(m_in_use_pool);
        
        m_in_use_pool = getFreePool();
        alloc_info.descriptorPool = m_in_use_pool;

        vk_check(vkAllocateDescriptorSets(
            m_device_p->logical(),
            &alloc_info,
            &out_set
        ));
        
    } else if (res != VK_SUCCESS) {
        vk_check(res);
    }

    return out_set;
}

// Clear all the descriptor set allocated from the pool
void DescriptorAllocator::clearPools()
{
    // Add the in use pool to the full pools list
    m_full_pools.push_back(m_in_use_pool);

    // Clear all the full pool
    for (auto pool : m_full_pools) {
        VkResult reset_res = vkResetDescriptorPool(
            m_device_p->logical(),
            pool,
            0
        );
        
        if (reset_res != VK_SUCCESS) {
            throw DescriptorAllocatorError(
                "Descriptor pool reset error: {}",
                vk_error_str(reset_res)
            );
        }

        m_free_pools.push_back(pool);
    }

    // Clear the full pool vector
    m_full_pools.clear();

    // Get a new in use pool
    m_in_use_pool = getFreePool();
}

// PRIVATE:

// Get new pool from the free pools list or create a new one
VkDescriptorPool DescriptorAllocator::getFreePool()
{
    VkDescriptorPool out_pool;
    
    // Create a new pool if none are available
    if (!m_free_pools.empty()) {
        out_pool = m_free_pools.back();
        m_free_pools.pop_back();
    } else {
        out_pool = createPool();
    }

    return out_pool;
}

// Create a new command pool
VkDescriptorPool DescriptorAllocator::createPool()
{
    // If the set per pool variable is set to 0 
    // make it equal to the stating set per pool constant
    if (m_set_per_pool == 0)
        m_set_per_pool = DescriptorAllocator::m_staring_set_per_pool;
    
    // Calculate the pool sizes
    std::vector<VkDescriptorPoolSize> pool_sizes;
    pool_sizes.reserve(m_ratios.size());
    
    for (PoolSizeRatio ratio : m_ratios) {
        VkDescriptorPoolSize size;
        size.type = ratio.type;
        size.descriptorCount = ratio.ratio * m_set_per_pool;

        pool_sizes.push_back(size);
    }
    
    // Create the output pool
    VkDescriptorPoolCreateInfo pool_info = { };
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    
    pool_info.flags = 0;
    pool_info.maxSets = m_set_per_pool;
    
    pool_info.poolSizeCount = (u32)pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    
    VkDescriptorPool out_pool;
    VkResult res = vkCreateDescriptorPool(
        m_device_p->logical(),
        &pool_info,
        m_device_p->allocator(),
        &out_pool
    );

    if (res != VK_SUCCESS) {
        throw DescriptorAllocatorError(
            "Create descriptor pool error: {}",
            vk_error_str(res)
        );
    }
    
    // Increase the set per pool variable for the next pool creation
    // (max size = 4016)
    m_set_per_pool *= 2;
    
    if (m_set_per_pool > 4016)
        m_set_per_pool = 4016;
    
    return out_pool;
}

// Destroy all the pools in the allocator
void DescriptorAllocator::destroyPools()
{
    // Add the in use pool to the full pools list
    m_full_pools.push_back(m_in_use_pool);

    for (auto pool : m_full_pools) {
        vkDestroyDescriptorPool(
            m_device_p->logical(), 
            pool,
            m_device_p->allocator()
        );
    }
    for (auto pool : m_free_pools) {
        vkDestroyDescriptorPool(
            m_device_p->logical(), 
            pool,
            m_device_p->allocator()
        );
    }

    m_full_pools.clear();
    m_free_pools.clear();

    m_in_use_pool = VK_NULL_HANDLE;
}

} // namespace cndt::vulkan
