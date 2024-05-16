#ifndef CNDT_VK_CONTEXT_H
#define CNDT_VK_CONTEXT_H

#include "renderer/vulkan/initialization/vkValidation.h"

#include "conduit/window/window.h"
#include "renderer/vulkan/deleteQueue.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace cndt::vulkan {

// Store the vulkan instance, surface, allocator and debug messenger 
struct Context {
public:
    // Initialize the vulkan context
    void initialize(
        const char* application_title,
        Window *window_p,
        const VkAllocationCallbacks *allocator_p
    );
    
    // Shutdown the vulkan context
    void shutdown();
    
private:
    // Initialize the vulkan instance
    void initInstance(
        const char* application_title,
        Window *window_p
    );
    // Shutdown the vulkan instance
    void shutdownInstance();

    // Create the vulkan surface
    void createSurface(Window *window_p);
    // Destroy the vulkan surface
    void destroySurface();
    
    // Initialize the vulkan debug messenger
    void initDebugerMessenger();
    // Shutdown the vulkan debug messenger
    void shutdownDebugerMessenger();
    
    // Get a list of required extension 
    std::vector<const char*> getRequiredExtension(Window *window);
    // Log the available extension for the vulkan instance
    void logAvailableExtension();
 
public:
    VkInstance instance;
    const VkAllocationCallbacks *allocator;

    VkSurfaceKHR surface;

private:
    #ifdef VK_DEBUG_MESSENGER
    VkDebugUtilsMessengerEXT m_debug_messenger;
    #endif

    // Context delete queue
    DeleteQueue m_delete_queue;
};

} // namespace cndt::vulkan

#endif
