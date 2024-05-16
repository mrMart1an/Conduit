#include "conduit/logging.h"
#include "conduit/window/window.h"

#include "renderer/vulkan/initialization/vkValidation.h"
#include "renderer/vulkan/initialization/vkContext.h"
#include "renderer/vulkan/vkExceptions.h"
#include "renderer/vulkan/vkUtils.h"

#include <functional>
#include <vulkan/vulkan.h>

#include <vector>
#include <format>
#include <vulkan/vulkan_core.h>

#include "buildConfig.h"

#ifdef VK_DEBUG_MESSENGER

// Vulkan debug callback
static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity_lvl,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data_p,
    void*
) {
    switch (severity_lvl) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
            cndt::log::app::debug("[vulkan] {}", callback_data_p->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
            cndt::log::app::info("[vulkan] {}", callback_data_p->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
            cndt::log::app::warn("[vulkan] {}", callback_data_p->pMessage);
            break;
        }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
            cndt::log::app::error("[vulkan] {}", callback_data_p->pMessage);
            break;
        }
        
        default:
            break;
    }

    return VK_FALSE;
}

#endif

namespace cndt::vulkan {

// Initialize the vulkan context
void Context::initialize(
    const char* application_title,
    Window *window_p,
    const VkAllocationCallbacks *allocator
) {
    log::core::debug("Initializing vulkan context");

    // Assign the custom allocator
    this->allocator = allocator;

    initInstance(application_title, window_p);

    createSurface(window_p);

    initDebugerMessenger();
}

// Shutdown the vulkan context
void Context::shutdown() 
{
    log::core::debug("Destroying vulkan context");
    m_delete_queue.callDeleter();
}

// Initialize the vulkan instance
void Context::initInstance(const char* application_title, Window *window_p)
{
    // App information data
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = application_title;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = CNDT_ENGINE_NAME;
    app_info.engineVersion = VK_MAKE_VERSION(
        CNDT_ENGINE_VERSION_MAJOR, 
        CNDT_ENGINE_VERSION_MINOR, 
        CNDT_ENGINE_VERSION_PATCH
    );
    app_info.apiVersion = VK_API_VERSION_1_3;

    // instance creation info
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    
    // Enable the required extensions
    std::vector<const char*> extensions = getRequiredExtension(window_p);

    create_info.enabledExtensionCount = extensions.size();
    create_info.ppEnabledExtensionNames = extensions.data();

    // Enable the required validation layers
    std::vector<const char*> layers = Validation::getLayers();
    
    create_info.enabledLayerCount = layers.size();
    create_info.ppEnabledLayerNames = layers.data();

    // Create the vulkan instance
    VkResult res = vkCreateInstance(
        &create_info, 
        allocator, 
        &instance
    );

    if (res != VK_SUCCESS) {
        throw InstanceInitError(
            std::format("vkCreateInstance error: {}", vk_error_str(res))
        );
    }
    
    // Log the available instance extensions
    logAvailableExtension();

    // Add the instance shutdown function to the deleter queue
    m_delete_queue.addDeleter(std::bind(&Context::shutdownInstance, this));
}

// Shutdown the vulkan instance
void Context::shutdownInstance()
{
    vkDestroyInstance(instance, allocator);
}

// Create the vulkan surface
void Context::createSurface(Window *window_p)
{
    // Get a vulkan surface from the window implementation
    surface = window_p->getVkSurface(instance, allocator);

    m_delete_queue.addDeleter(std::bind(&Context::destroySurface, this));
}

// Destroy the vulkan surface
void Context::destroySurface() 
{
    vkDestroySurfaceKHR(instance, surface, allocator);    
}

// Initialize the vulkan debug messenger
void Context::initDebugerMessenger()
{
    #ifdef VK_DEBUG_MESSENGER
    
    // Create the debug message callback    
    VkDebugUtilsMessengerCreateInfoEXT create_debug_info{};
    create_debug_info.sType = 
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_debug_info.messageSeverity = 
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_debug_info.messageType = 
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    
    create_debug_info.pfnUserCallback = vk_debug_callback;
    create_debug_info.pUserData = NULL; 

    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT"
        );
    
    if (func != NULL) {
        vk_check(func(
            instance,
            &create_debug_info,
            allocator,
            &m_debug_messenger
        ));
    } else {
        log::core::error(
            "Debug messenger initialization error: vkGetInstanceProcAddr error"
        );
    }

    // Add debug messenger shutdown function to the delete queue 
    m_delete_queue.addDeleter(
        std::bind(&Context::shutdownDebugerMessenger, this)
    );
    
    #endif
}

// Shutdown the vulkan debug messenger
void Context::shutdownDebugerMessenger() 
{
    #ifdef VK_DEBUG_MESSENGER
    
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT"
        );
    
    if (func != NULL) {
        func(instance, m_debug_messenger, allocator);
    } else {
        log::core::error(
            "Debug messenger shutdown error: vkGetInstanceProcAddr error"
        );
    }
    
    #endif
}

// Get a list of required extension 
std::vector<const char*> Context::getRequiredExtension(Window *window)
{
    // Get the window vulkan required extension
    std::vector<const char*> out_extension = window->getVkExtensions();

    // If the debug messenger is enable add the required extension 
    #ifdef VK_DEBUG_MESSENGER 
    out_extension.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    return out_extension;
}

// Log the available extension for the vulkan instance
void Context::logAvailableExtension() 
{
    u32 ext_count = 0;
    vk_check(vkEnumerateInstanceExtensionProperties(NULL, &ext_count, NULL));

    // Allocate the extension vector
    std::vector<VkExtensionProperties> extensions(ext_count);
    
    vk_check(vkEnumerateInstanceExtensionProperties(
        NULL,
        &ext_count,
        extensions.data()
    ));

    // Print the supported extension for debug purpose 
    log::core::trace("Supported Vulkan extensions:");

    for (u32 i = 0; i < ext_count; i++) {
        log::core::trace(
            "Extensions: (versions: {}) {}",
            extensions[i].specVersion,
            extensions[i].extensionName
        );
    }
}

} // namespace cndt::vulkan
