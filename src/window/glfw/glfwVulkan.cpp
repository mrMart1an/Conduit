#include "buildConfig.h"

#ifdef CNDT_VULKAN_BACKEND

#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "glfwWindow.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <format>

namespace cndt::glfw {

// Retrieve a vulkan surface
VkSurfaceKHR GlfwWindow::getVkSurface(
    VkInstance instance,
    const VkAllocationCallbacks *allocator_p
) {
    VkSurfaceKHR out_surface;

    VkResult res = glfwCreateWindowSurface(
        instance,
        m_glfw_window,
        allocator_p,
        &out_surface
    );

    if (res != VK_SUCCESS) {
        throw vulkan::SurfaceInitError(std::format(
            "Glfw vulkan surface creation failed: {}", 
            vulkan::vk_error_str(res)
        ));
    }

    return out_surface;
}

// Fetch the required vulkan extensions
std::vector<const char*> GlfwWindow::getVkExtensions() 
{
    u32 ext_count;
    const char **out_exts = glfwGetRequiredInstanceExtensions(&ext_count);

    // Check if the call was successful
    if (out_exts == NULL) {
        throw vulkan::UnexpectedError(
            "glfwGetRequiredInstanceExtensions returned NULL"
        );
    }

    if (ext_count < 1) {
        throw vulkan::UnexpectedError(
            "glfwGetRequiredInstanceExtensions zero extensions"
        );
    }

    // Add the extensions to the output vector
    std::vector<const char*> out_vector(ext_count);
    
    for (u32 i = 0; i < ext_count; i++) {
        out_vector.at(i) = out_exts[i];
    }
    
    return out_vector;
}

} // namespace cndt::glfw

#else

#endif
