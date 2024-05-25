#ifndef CNDT_VK_SWAP_CHAIN_H
#define CNDT_VK_SWAP_CHAIN_H

#include "conduit/defines.h"

#include "renderer/vulkan/vkContext.h"
#include "renderer/vulkan/vkDevice.h"

#include <vector>

#include <vulkan/vulkan_core.h>

namespace cndt::vulkan {

// Vulkan swap chain abstraction
class SwapChain {
private:
    // Store swap chain information for initialization
    class Details {
    public:
        Details(Context &context, Device &device);
        ~Details() = default;

        // Return the swap chain capabilities
        VkSurfaceCapabilitiesKHR capabilities() const {
            return m_capabilities;
        }
        
        // Return a reference to the supported swap chain formats vector
        const std::vector<VkSurfaceFormatKHR>& formats() const {
            return m_formats;
        }
        // Return a reference to the supported swap chain present modes vector
        const std::vector<VkPresentModeKHR>& present_modes() const {
            return m_present_modes;
        };  

    private:
        VkSurfaceCapabilitiesKHR m_capabilities;

        std::vector<VkSurfaceFormatKHR> m_formats;
        std::vector<VkPresentModeKHR> m_present_modes;  
    };

public:
    // Initialize the swap chain
    // The actual number of frame in flight might differ from the requested one
    // the minimum number of frame in flight is 1
    void initialize(
        Context &context,
        Device &device,
        
        u32 frame_in_flight,
        
        u32 width, u32 height,
        bool v_sync
    );
    
    // Reinitialize an out dated the swap chain
    void reinitialize(
        Context &context,
        Device &device,
        
        u32 width, u32 height
    );

    // Shutdown the swap chain
    void shutdown(Context &context, Device &device);
    
    // Enable or disable v-sync
    void setVsync(Context &context, Device &device, bool v_sync);

    // Store the index to the next swap chain image to present after rendering
    // Return true if the image was acquired successfully
    bool acquireNextImage(
        Device &device,
        VkSemaphore image_available,
        VkFence fence
    );   

    // Present the current swap chain image
    // Return true if the image was presented successfully
    bool presentImage(Device &device, VkSemaphore render_done);
    
    /*
     *
     *      Getter
     *
     * */

    // Get the index to the next swap chain image to present
    u32 currentImage() const { return m_current_image; }
    
    // Get the index to the current frame in flight
    u32 currentFrame() const { return m_current_frame; }

    // Return true if the swap chain is out of date and need to be recreated
    bool outOfDate() const { return m_outdated; }

private:
    // Chose the swap chain format among the available ones
    VkSurfaceFormatKHR chooseFormat(Details &details);
    // Chose the swap chain present mode among the available ones
    VkPresentModeKHR choosePresentMode(Details &details, bool v_sync);
    // Chose the swap chain extent among the available ones
    VkExtent2D chooseExtent(Details &details, u32 width, u32 height);

    // Create the swap chain image views
    void createImageViews(Context &context, Device &device);

    // Destroy the swap chain image views
    void destroyImageViews(Context &context, Device &device);

private:
    VkSwapchainKHR m_handle;

    VkFormat m_format;
    VkExtent2D m_extent;
    bool m_v_sync;

    u32 m_frame_in_flight;

    u32 m_image_count;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_image_views;
    
    u32 m_current_frame;
    u32 m_current_image;

    bool m_outdated;
};

} // namespace cndt::vulkan

#endif
