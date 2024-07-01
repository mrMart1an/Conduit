#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/vkDevice.h"

#include "renderer/vulkan/sync/vkFence.h"

#include <vulkan/vulkan_core.h>

#include <fmt/format.h>

namespace cndt::vulkan {

// Wait to the fence to be signaled
void Fence::wait(u64 timeout)
{
    VkResult res = vkWaitForFences(
        m_device_p->logical,
        1, &m_handle,
        VK_TRUE, 
        timeout
    );

    if (res != VK_SUCCESS) {
        throw FenceWaitError(fmt::format(
            "Vulkan fence wait error {}",
            vk_error_str(res)
        ));
    }
}

// Reset the fence to the non signaled state
void Fence::reset()
{
    VkResult res = vkResetFences(
        m_device_p->logical,
        1, &m_handle
    );
    
    if (res != VK_SUCCESS) {
        throw FenceResetError(fmt::format(
            "Vulkan fence reset error {}",
            vk_error_str(res)
        ));
    }   
}

};
