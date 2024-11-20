#ifndef CNDT_VK_SWAP_CHAIN_H
#define CNDT_VK_SWAP_CHAIN_H

#include "conduit/defines.h"

#include "renderer/vulkan/storage/vkImage.h"
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
        Details(Context *context_p, Device *device_p);
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
        bool v_sync,

        GpuImage::Info::UsageEnum swap_chain_image_usage
    );
    
    // Reinitialize the swap chain, 
    // if the width or height value are not provided the current surface 
    // extent value are used, if the v-sync value is not provided the 
    // current v-sync mode is used
    void reinitialize(
        std::optional<u32> width = std::nullopt, 
        std::optional<u32> height = std::nullopt, 

        std::optional<bool> v_sync = std::nullopt,

        std::optional<GpuImage::Info::UsageEnum> 
        swap_chain_image_usage = std::nullopt
    );

    // Shutdown the swap chain
    void shutdown();
    
    // Enable or disable v-sync
    void setVsync(bool v_sync);

    // Inform the swap chain that the Vulkan surface extent changed
    // also make the swap chain out of date
    void setSurfaceExtent(u32 width, u32 height);

    // Acquire a new swap chain image and return a const pointer to it
    // if successful, recreate the swap chain if marked as out of date.
    // If the operation is unsuccessful return nullptr and make the 
    // swap chain as out of date
    //
    // The given semaphore and fence are signaled when an image is acquired
    const VulkanImage* acquireNextImage(
        VkSemaphore image_available,
        VkFence fence
    );

    // Present the current swap chain image
    void presentImage(VkSemaphore render_done);

    /*
     *
     *      Getter
     *
     * */

    // Return the swap chain image format
    GpuImage::Info::Format format() const { return m_format; }

    // Return the swap chain vulkan format
    VkFormat vkFormat() const { return m_vk_format; }
    
    // Return the swap chain image extent
    VkExtent2D extent() const { return m_extent; }

    // Return true if v-sync is enabled
    bool vSync() const { return m_v_sync; }

    // Return the number of swap chain images
    u32 imageCount() const { return m_image_count; }
    
    // Return true if the swap chain is out of date and need to be recreated
    bool outOfDate() const { return m_outdated; }

private:
    // Private initialization code, used by the initialization and 
    // reinitialization function
    void initializeSwapChain(
        u32 frame_in_flight,
        
        u32 width, u32 height,
        bool v_sync,

        GpuImage::Info::UsageEnum swap_chain_image_usage
    );

    // Private shutdown code, used by the shutdown and 
    // reinitialization function
    void shutdownSwapChain();

    // Choose the number of swap chain images
    u32 chooseMinImagesCount(Details &details, u32 frame_in_flight);
    // Chose the swap chain format among the available ones
    VkSurfaceFormatKHR chooseFormat(Details &details);
    // Chose the swap chain present mode among the available ones
    VkPresentModeKHR choosePresentMode(Details &details, bool v_sync);
    // Chose the swap chain extent among the available ones
    VkExtent2D chooseExtent(Details &details, u32 width, u32 height);

    // Create the swap chain image views
    void createImages();

    // Destroy the swap chain image views
    void destroyImages();

private:
    VkSwapchainKHR m_handle;

    u32 m_frame_in_flight;
    // Current surface extent
    VkExtent2D m_surface_extent;

    // Swap chain image current format 
    GpuImage::Info::Format m_format;
    VkFormat m_vk_format;

    // Swap chain image current extent 
    VkExtent2D m_extent;
    bool m_v_sync;

    GpuImage::Info::UsageEnum m_swap_chain_image_usage;

    // Store the swap chain Vulkan images
    u32 m_image_count;
    std::vector<VulkanImage> m_images;

    // Index to the current image in the images vector
    u32 m_current_image;

    bool m_outdated;

    // Pointer to the device owing the swap chain 
    Device* m_device_p;
    // Pointer to the context of the renderer owing the swap chain
    Context* m_context_p;
};

} // namespace cndt::vulkan

#endif
