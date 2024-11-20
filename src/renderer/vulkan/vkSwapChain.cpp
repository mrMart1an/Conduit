#include "conduit/logging.h"

#include "renderer/vulkan/utils/vkExceptions.h"
#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkSwapChain.h"

#include <algorithm>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Private initialization code, used by the initialization and 
// reinitialization function
void SwapChain::initializeSwapChain(
    u32 frame_in_flight,
    
    u32 width, u32 height,
    bool v_sync,

    GpuImage::Info::UsageEnum swap_chain_image_usage
) {
    // Get the number of images in the swap chains
    if (frame_in_flight < 1) {
        throw SwapChainInitError(
            "Frame in flight need to be at least 1"
        );
    }

    // Get the swap chain configurations
    Details details(m_context_p, m_device_p);

    VkSurfaceFormatKHR surface_format = chooseFormat(details);
    VkPresentModeKHR present_mode = choosePresentMode(details, v_sync);
    VkExtent2D extent = chooseExtent(details, width, height);
    u32 min_image_count = chooseMinImagesCount(details, frame_in_flight);

    // Set up the swap chain create info
    VkSwapchainCreateInfoKHR create_info = { };
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = m_context_p->surface;

    create_info.minImageCount = min_image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = m_device_p->getVkImageUsage(
        swap_chain_image_usage
    );

    // Get the queue indices
    // TODO Handle compute and transfer queue
    Device::QueueFamilyIndices indices = m_device_p->queueIndices(); 

    u32 queue_indices[] = { 
        indices.general().first, 
        indices.present().first 
    };

    // If the graphic and presentation are performed by the same queue
    // Use exclusive sharing mode
    if (indices.general().first != indices.present().first) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    // Specify the transformation to apply to the chain
    create_info.preTransform = details.capabilities().currentTransform;

    // Ignore the alpha channel
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    // Create the swap chain
    VkResult res = vkCreateSwapchainKHR(
        m_device_p->logical(), 
        &create_info, 
        m_context_p->allocator, 
        &m_handle
    );

    if (res != VK_SUCCESS) {
        throw SwapChainInitError(
            "Swap chain creation error: {}",
            vk_error_str(res)
        );
    }

    // Store swap chain info, MUST be set before creating views
    m_vk_format = surface_format.format; 
    if (m_vk_format == VK_FORMAT_B8G8R8A8_SRGB)
        m_format = GpuImage::Info::Format::B8G8R8A8_SRGB;
    else if (m_vk_format == VK_FORMAT_B8G8R8A8_UNORM)
        m_format = GpuImage::Info::Format::B8G8R8A8_UNORM;
    else 
        throw UnexpectedError("Swap chain using unknown format");

    m_swap_chain_image_usage = swap_chain_image_usage;
    m_surface_extent = extent;
    m_extent = extent;
    m_v_sync = v_sync;

    m_frame_in_flight = frame_in_flight;
    m_current_image = 0;
    
    m_outdated = false;

    // Create the image views 
    createImages();
}

// Private shutdown code, used by the shutdown and 
// reinitialization function
void SwapChain::shutdownSwapChain()
{
    // Wait for all the device operation to finish
    vk_check(vkDeviceWaitIdle(m_device_p->logical()));

    // Destroy the swap chain image views
    destroyImages();
    
    // Destroy the swap chain
    vkDestroySwapchainKHR(
        m_device_p->logical(),
        m_handle,
        m_context_p->allocator
    );
}

// Initialize the swap chain
void SwapChain::initialize(
    Context &context,
    Device &device,
    
    u32 frame_in_flight,

    u32 width, u32 height,
    bool v_sync,

    GpuImage::Info::UsageEnum swap_chain_image_usage
) {
    log::core::debug("Initializing vulkan swap chain");

    // Store the device and context pointer
    m_device_p = &device;
    m_context_p = &context;

    initializeSwapChain(
        frame_in_flight,
        width, height,
        v_sync, 
        swap_chain_image_usage
    );
}

// Reinitialize an out dated the swap chain
void SwapChain::reinitialize(
    std::optional<u32> width, 
    std::optional<u32> height, 

    std::optional<bool> v_sync,

    std::optional<GpuImage::Info::UsageEnum> swap_chain_image_usage
) {
    log::core::debug("Reinitializing vulkan swap chain");

    // Shutdown and reinitialize the swap chain
    shutdownSwapChain();

    initializeSwapChain(
        m_frame_in_flight,

        width.value_or(m_surface_extent.width),
        height.value_or(m_surface_extent.height),
        v_sync.value_or(m_v_sync),

        swap_chain_image_usage.value_or(m_swap_chain_image_usage)
    );
}

// Shutdown the swap chain
void SwapChain::shutdown()
{
    log::core::debug("Shutting down vulkan swap chain");

    shutdownSwapChain();
}

// Enable or disable v-sync
void SwapChain::setVsync(bool v_sync)
{
    // Shutdown and reinitialize the swap chain
    reinitialize(
        std::nullopt, std::nullopt,
        v_sync
    );
}

// Inform the swap chain that the Vulkan surface extent changed
// also make the swap chain out of date
void SwapChain::setSurfaceExtent(u32 width, u32 height)
{
    m_outdated = true;

    m_surface_extent.width = width;
    m_surface_extent.height = height;
}

/*
 *
 *      Presentation engine functions
 *
 * */

// Store the index to the next swap chain image to present after rendering
const VulkanImage* SwapChain::acquireNextImage(
    VkSemaphore image_available,
    VkFence fence
) {
    // Recreate the swap chain if out dated
    if (m_outdated)
        reinitialize();
    
    // Acquire the image
    VkResult res = vkAcquireNextImageKHR(
        m_device_p->logical(), 
        m_handle, 
        UINT64_MAX,
        image_available, 
        fence, 
        &m_current_image
    );

    // Check the result and recreate the swap chain if it's out of date
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::acquireNextImage -> Vulkan swap chain out of date"
        );
        
        return nullptr;
        
    } else if (res == VK_SUBOPTIMAL_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::acquireNextImage -> Vulkan swap chain suboptimal"
        );
        
    } else if (res != VK_SUCCESS) {
        throw SwapChainImageAcquireError(
            "Swap chain image acquisition error: {}",
            vk_error_str(res)
        );
    }
    
    return &m_images[m_current_image];
}

// Present the current swap chain image
void SwapChain::presentImage(VkSemaphore render_done)
{
    VkPresentInfoKHR present_info = { };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_handle;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_done;
    present_info.pImageIndices = &m_current_image;

    VkResult res = vkQueuePresentKHR(
        m_device_p->presentQueue().handle(), 
        &present_info
    );
    
    // Check the result and recreate the swap chain if it's out of date
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::presentImage -> Vulkan swap chain out of date"
        );
        
    } else if (res == VK_SUBOPTIMAL_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::presentImage -> Vulkan swap chain suboptimal"
        );
        
    } else if (res != VK_SUCCESS) {
        throw SwapChainPresentError(
            "Swap chain image presentation error: {}",
            vk_error_str(res)
        );
    }
}

/*
 *
 *      Images functions
 *
 * */

// Create the swap chain image views
void SwapChain::createImages() {
    // Retrieve the swap chain images
    VkResult res_retrive = vkGetSwapchainImagesKHR(
        m_device_p->logical(), 
        m_handle, 
        &m_image_count, 
        VK_NULL_HANDLE
    );
    if (res_retrive != VK_SUCCESS) {
        throw SwapChainImageAcquireError(
            "Vulkan swap chain image acquisition error"
        );
    }
    
    std::vector<VkImage> images(m_image_count);
    res_retrive = vkGetSwapchainImagesKHR(
        m_device_p->logical(), 
        m_handle, 
        &m_image_count, 
        images.data()
    );
    if (res_retrive != VK_SUCCESS) {
        throw SwapChainImageAcquireError(
            "Vulkan swap chain image acquisition error"
        );
    }

    log::core::trace("Swap chain image count: {}", m_image_count);

    // Create the swap chain image
    m_images.reserve(images.size());

    // Get extent
    GpuImage::Extent extent;
    extent.width = m_extent.width;
    extent.height = m_extent.height;

    for (auto &image : images) {
        VulkanImage vk_image = m_device_p->createSwapChainImage(
            image, 

            m_swap_chain_image_usage,
            m_format, 
            extent
        );

        // Add the view to the vector
        m_images.push_back(vk_image);
    }
}

// Destroy the swap chain image views
void SwapChain::destroyImages()
{
    for (auto& image : m_images) {
        m_device_p->destroySwapChainImage(image);
    }    

    m_images.clear();
}

/*
*
*       Details functions
*
* */

// Choose the number of swap chain images
u32 SwapChain::chooseMinImagesCount(Details &details, u32 frame_in_flight)
{
    u32 min_image_count = frame_in_flight + 1;

    // If max image count is 0 there is no maximum
    if (details.capabilities().maxImageCount > 0) {
        min_image_count = std::clamp(
            min_image_count,
            details.capabilities().minImageCount,
            details.capabilities().maxImageCount
        );
    } else {
        min_image_count = std::clamp(
            min_image_count,
            details.capabilities().minImageCount,
            UINT32_MAX
        );
    }

    // Check if image count is past the maximum (0 means no maximum)
    if (
        details.capabilities().maxImageCount > 0 &&
        min_image_count > details.capabilities().maxImageCount
    ) {
        min_image_count = details.capabilities().maxImageCount;
    }

    return min_image_count;
}

// Chose the swap chain format among the available ones
VkSurfaceFormatKHR SwapChain::chooseFormat(Details &details)
{
    // Pick srgb format if available
    for (u32 i = 0; i < details.formats().size(); i++) {
        VkSurfaceFormatKHR available_format = details.formats().at(i);

        bool srgb_format = available_format.format == VK_FORMAT_B8G8R8A8_SRGB;
        bool srgb_color_space = 
            available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        if (srgb_format && srgb_color_space) {
            log::core::trace("Vulkan swap chain: using SRGB format");
            
            return available_format;
        }
    }

    // Fallback to unorm if srgb is unavailable 
    for (u32 i = 0; i < details.formats().size(); i++) {
        VkSurfaceFormatKHR available_format = details.formats().at(i);

        bool unorm_format = 
            available_format.format == VK_FORMAT_B8G8R8A8_UNORM;

        if (unorm_format) {
            log::core::trace("Vulkan swap chain: using UNORM format");
            
            return available_format;
        }
    }

    throw SwapChainInitError(
        "Vulkan swpa chain init error: no supported format found"
    );
}

// Chose the swap chain present mode among the available ones
VkPresentModeKHR SwapChain::choosePresentMode(Details &details, bool v_sync)
{
    // WARNING!!! This code does not work at the moment,
    // present mode will always be FIFO
   
    VkPresentModeKHR desired_mode = VK_PRESENT_MODE_MAILBOX_KHR;

    if (!v_sync)
       desired_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    
    // If mailbox present mode is supported use it
    for (usize i = 0; i < details.present_modes().size(); i++) {
        VkPresentModeKHR available_mode = details.present_modes().at(i);

        if (available_mode == desired_mode) {
            // TODO: Fix how present mode is handle, default to FIFO for now
            return VK_PRESENT_MODE_FIFO_KHR;
        }
    }

    // Fallback to FIFO otherwise
    return VK_PRESENT_MODE_FIFO_KHR;   
}

// Chose the swap chain extent among the available ones
VkExtent2D SwapChain::chooseExtent(Details &details, u32 width, u32 height)
{
    VkSurfaceCapabilitiesKHR capabilities = details.capabilities();

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actual_extent = { };

        // Clamp the values
        actual_extent.width = std::clamp(
            width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width
        );

        actual_extent.height = std::clamp(
            height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height
        );
        
        return actual_extent;
    }
}

// Swap chain details constructor
SwapChain::Details::Details(Context *context_p, Device *device_p) 
{
    // Get swap chain capabilities
    vk_check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device_p->physical(), 
        context_p->surface, 
        &m_capabilities
    ));

    // Get available formats
    u32 format_count;
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device_p->physical(),
        context_p->surface,
        &format_count, 
        VK_NULL_HANDLE
    ));

    m_formats.resize(format_count);
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device_p->physical(),
        context_p->surface,
        &format_count, 
        m_formats.data()
    ));
    
    // Get available present mode
    u32 present_mode_count;
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device_p->physical(),
        context_p->surface,
        &present_mode_count, 
        VK_NULL_HANDLE
    ));
    
    m_present_modes.resize(present_mode_count);
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device_p->physical(),
        context_p->surface,
        &present_mode_count, 
        m_present_modes.data()
    ));
}

}
