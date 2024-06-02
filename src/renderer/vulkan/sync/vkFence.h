#ifndef CNDT_VK_FENCE_H
#define CNDT_VK_FENCE_H

#include "conduit/defines.h"

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

class Device;

class Fence {
    friend class Device;

public:
    Fence() = default;

    // Wait to the fence to be signaled
    void wait(u64 timeout = UINT64_MAX);

    // Reset the fence to the non signaled state
    void reset();

    // Return the vulkan fence handle
    VkFence hande() const { return m_handle; }
    
private:
    VkFence m_handle;

    // A pointer to the device that own the fence 
    Device *m_device_p;
};

} // namespace cndt::vulkan

#endif
