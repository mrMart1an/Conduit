#include "conduit/logging.h"

#include "renderer/vulkan/utils/vkUtils.h"
#include "renderer/vulkan/utils/vkUtils.h"

#include "renderer/vulkan/vkSwapChain.h"

#include <algorithm>

#include <vulkan/vulkan_core.h>
#include <fmt/format.h>

namespace cndt::vulkan {

// Initialize the swap chain
void SwapChain::initialize(
    Context &context,
    Device &device,
    
    u32 frame_in_flight,

    u32 width, u32 height,
    bool v_sync
) {
    log::core::debug("Initializing vulkan swap chain");

    Details details(context, device);

    // Get the swap chain configurations
    VkSurfaceFormatKHR surface_format = chooseFormat(details);
    VkPresentModeKHR present_mode = choosePresentMode(details, v_sync);
    VkExtent2D extent = chooseExtent(details, width, height);

    // Get the number of images in the swap chains
    if (frame_in_flight < 1)
        frame_in_flight = 1;
    
    u32 image_count = frame_in_flight + 1;

    // If max image count is 0 there is no maximum
    if (details.capabilities().maxImageCount > 0) {
        image_count = std::clamp(
            image_count,
            details.capabilities().minImageCount,
            details.capabilities().maxImageCount
        );
    } else {
        image_count = std::clamp(
            image_count,
            details.capabilities().minImageCount,
            UINT32_MAX
        );
    }

    // Check if image count is past the maximum (0 means no maximum)
    if (
        details.capabilities().maxImageCount > 0 &&
        image_count > details.capabilities().maxImageCount
    ) {
        image_count = details.capabilities().maxImageCount;
    }

    // Set up the swap chain create info
    VkSwapchainCreateInfoKHR create_info = { };
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = context.surface;

    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Get the queue indices
    Device::QueueFamilyIndices indices = device.queueIndices(); 

    u32 queue_indices[] = { 
        indices.graphicsIndex(), 
        indices.presentIndex() 
    };

    // If the graphic and presentation are performed by the same queue
    // Use exclusive sharing mode
    if (indices.graphicsIndex() != indices.presentIndex()) {
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
        device.logical, 
        &create_info, 
        context.allocator, 
        &m_handle
    );

    if (res != VK_SUCCESS) {
        throw SwapChainInitError(fmt::format(
            "Swap chain creation error: {}",
            vk_error_str(res)
        ));
    }

    // Retrieve the swap chain images
    vk_check(vkGetSwapchainImagesKHR(
        device.logical, 
        m_handle, 
        &m_image_count, 
        VK_NULL_HANDLE
    ));
    
    m_images.resize(m_image_count);
    vk_check(vkGetSwapchainImagesKHR(
        device.logical, 
        m_handle, 
        &m_image_count, 
        m_images.data()
    ));

    log::core::trace("Swap chain image count: {}", m_image_count);
    
    // Store swap chain info
    m_format = surface_format.format; // MUST be set before creating views
    m_extent = extent;
    m_v_sync = v_sync;

    m_frame_in_flight = m_image_count - 1;
    m_current_image = 0;
    m_current_frame = 0;
    
    m_outdated = false;

    // Create the image views 
    createImageViews(context, device);
}

// Reinitialize an out dated the swap chain
void SwapChain::reinitialize(
    Context &context,
    Device &device,
    
    u32 width, u32 height
) {
    log::core::debug("Reinitializing vulkan swap chain");

    // Wait for all the device operation to finish
    vk_check(vkDeviceWaitIdle(device.logical));

    // Shutdown and reinitialize the swap chain
    shutdown(context, device);

    initialize(
        context,
        device,

        m_frame_in_flight,
        width,
        height,
        m_v_sync
    );
}

// Shutdown the swap chain
void SwapChain::shutdown(Context &context, Device &device)
{
    log::core::debug("Shutting down vulkan swap chain");

    // Destroy the swap chain image views
    destroyImageViews(context, device);
    
    // Destroy the swap chain
    vkDestroySwapchainKHR(
        device.logical,
        m_handle,
        context.allocator
    );
}

// Enable or disable v-sync
void SwapChain::setVsync(Context &context, Device &device, bool v_sync)
{
    // Shutdown and reinitialize the swap chain
    shutdown(context, device);

    initialize(
        context,
        device,
        
        m_frame_in_flight,
        m_extent.width,
        m_extent.height,
        v_sync
    );
}

// Store the index to the next swap chain image to present after rendering
bool SwapChain::acquireNextImage(
    Device &device,
    VkSemaphore image_available,
    VkFence fence
) {
    VkResult res = vkAcquireNextImageKHR(
        device.logical, 
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
        
        return false;
        
    } else if (res == VK_SUBOPTIMAL_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::acquireNextImage -> Vulkan swap chain suboptimal"
        );
        
        return true;
        
    } else if (res != VK_SUCCESS) {
        throw SwapChainImageAcquireError(fmt::format(
            "Swap chain image acquisition error: {}",
            vk_error_str(res)
        ));
    }
    
    return true;
}

// Present the current swap chain image
bool SwapChain::presentImage(Device &device, VkSemaphore render_done)
{
    VkPresentInfoKHR present_info = { };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_handle;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_done;
    present_info.pImageIndices = &m_current_image;

    VkResult res = vkQueuePresentKHR(device.present_queue, &present_info);
    
    // Check the result and recreate the swap chain if it's out of date
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::presentImage -> Vulkan swap chain out of date"
        );
        
        return false;
        
    } else if (res == VK_SUBOPTIMAL_KHR) {
        m_outdated = true;
        
        log::core::trace(
            "SwapChain::presentImage -> Vulkan swap chain suboptimal"
        );
        
        return false;
        
    } else if (res != VK_SUCCESS) {
        throw SwapChainPresentError(fmt::format(
            "Swap chain image presentation error: {}",
            vk_error_str(res)
        ));
    }
    
    // Update the current frame counter
    m_current_frame = (m_current_frame + 1) % m_frame_in_flight;
    
    return true;
}

// Create the swap chain image views
void SwapChain::createImageViews(Context &context, Device &device)
{
    m_image_views.reserve(m_images.size());

    for (auto &image : m_images) {
        // Create the image view
        VkImageView image_view;
        VkImageViewCreateInfo view_info = { };
        
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = m_format;
        
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;
        
        // Create the image view
        VkResult res = vkCreateImageView(
            device.logical,
            &view_info,
            context.allocator,
            &image_view
        );
        
        if (res) {
            throw SwapChainViewError(fmt::format(
                "Swap chain image view creation error: {}",
                vk_error_str(res)
            ));
        }

        // Add the view to the vector
        m_image_views.push_back(image_view);
    }
}

// Destroy the swap chain image views
void SwapChain::destroyImageViews(Context &context, Device &device)
{
    for (u32 i = 0; i < m_image_count; i++) {
        VkImageView view = m_image_views.back();
        m_image_views.pop_back();
        
        vkDestroyImageView(device.logical, view, context.allocator);
    }    
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
            log::core::trace("Vulkan swap chain: SRGB format supported");
            
            return available_format;
        }
    }

    // Fallback to the first available format otherwise
    return details.formats().at(0);
}

// Chose the swap chain present mode among the available ones
VkPresentModeKHR SwapChain::choosePresentMode(Details &details, bool v_sync)
{
    if (!v_sync)
       return VK_PRESENT_MODE_IMMEDIATE_KHR;
    
    // If mailbox present mode is supported use it
    for (usize i = 0; i < details.present_modes().size(); i++) {
        VkPresentModeKHR available_mode = details.present_modes().at(i);

        if (available_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return available_mode;
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
SwapChain::Details::Details(Context &context, Device &device) 
{
    // Get swap chain capabilities
    vk_check(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device.physical, 
        context.surface, 
        &m_capabilities
    ));

    // Get available formats
    u32 format_count;
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device.physical,
        context.surface,
        &format_count, 
        VK_NULL_HANDLE
    ));

    m_formats.resize(format_count);
    vk_check(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device.physical,
        context.surface,
        &format_count, 
        m_formats.data()
    ));
    
    // Get available present mode
    u32 present_mode_count;
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device.physical,
        context.surface,
        &present_mode_count, 
        VK_NULL_HANDLE
    ));
    
    m_present_modes.resize(present_mode_count);
    vk_check(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device.physical,
        context.surface,
        &present_mode_count, 
        m_present_modes.data()
    ));
}

}
