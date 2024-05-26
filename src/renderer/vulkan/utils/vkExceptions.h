#ifndef CNDT_VK_EXCEPTIONS
#define CNDT_VK_EXCEPTIONS

#include "conduit/renderer/renderer.h"

namespace cndt::vulkan {

/*
 *
 *      Vulkan renderer exception type
 *
 * */

// Renderer generic exception
class VulkanException : public RendererException {
public:
    VulkanException(std::string_view message) : 
        RendererException(message, RendererBackend::Vulkan)
    { }
    VulkanException() : 
        RendererException("Renderer exception", RendererBackend::Vulkan) 
    { }
};

// Unexpected vulkan error
class UnexpectedError : public VulkanException {
public:
    UnexpectedError(std::string_view message) : 
        VulkanException(message)
    { }
    UnexpectedError() : 
        VulkanException("Unexpected vulkan error") 
    { }
};

/*
 *
 *      Initialization errors
 *
 * */

// Initialization stage error
class InitializationError : public VulkanException {
public:
    InitializationError(std::string_view message) : 
        VulkanException(message)
    { }
    InitializationError() : 
        VulkanException("Vulkan initialization error")
    { }
};

// Vulkan instance initialization error
class InstanceInitError : public InitializationError {
public:
    InstanceInitError(std::string_view message) : 
        InitializationError(message)
    { }
};

// Vulkan surface initialization error
class SurfaceInitError : public InitializationError {
public:
    SurfaceInitError(std::string_view message) : 
        InitializationError(message)
    { }
};

// Vulkan physical device initialization error
class PhysicalDeviceError : public InitializationError {
public:
    PhysicalDeviceError(std::string_view message) : 
        InitializationError(message)
    { }
};

// Vulkan logical device initialization error
class DeviceInitError : public InitializationError {
public:
    DeviceInitError(std::string_view message) : 
        InitializationError(message)
    { }
};

/*
 *
 *      Sync object exception
 *
 * */

// Vulkan generic sync object exception
class SyncObjectException : public VulkanException {
public:
    SyncObjectException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan fence initialization error
class FenceInitError : public SyncObjectException {
public:
    FenceInitError(std::string_view message) : 
        SyncObjectException(message)
    { }
};

// Vulkan semaphore initialization error
class SemaphoreInitError : public SyncObjectException {
public:
    SemaphoreInitError(std::string_view message) : 
        SyncObjectException(message)
    { }
};

/*
 *
 *      Command pool exception
 *
 * */

// Vulkan command pool generic exception
class CommandPoolException : public VulkanException {
public:
    CommandPoolException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan command pool initialization error
class CommandPoolInitError : public CommandPoolException {
public:
    CommandPoolInitError(std::string_view message) : 
        CommandPoolException(message)
    { }
};

// Vulkan command pool reset error
class CommandPoolResetError : public CommandPoolException {
public:
    CommandPoolResetError(std::string_view message) : 
        CommandPoolException(message)
    { }
};

/*
 *
 *      Device errors
 *
 * */

// Generic vulkan device exception
class DeviceException : public VulkanException {
public:
    DeviceException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan device memory allocation exception
class DeviceMemoryError : public DeviceException {
public:
    DeviceMemoryError(std::string_view message) : 
        DeviceException(message)
    { }
};

/*
 *
 *      Device buffers errors
 *
 * */

// Generic buffer exception
class BufferException : public VulkanException {
public:
    BufferException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan buffer creation exception
class BufferCreateError : public BufferException {
public:
    BufferCreateError(std::string_view message) : 
        BufferException(message)
    { }
};

// Vulkan buffer bind exception
class BufferBindError : public BufferException {
public:
    BufferBindError(std::string_view message) : 
        BufferException(message)
    { }
};

// Vulkan buffer map exception
class BufferMapError : public BufferException {
public:
    BufferMapError(std::string_view message) : 
        BufferException(message)
    { }
};

/*
 *
 *      Device image errors
 *
 * */

// Generic image exception
class ImageException : public VulkanException {
public:
    ImageException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan image creation exception
class ImageCreateError : public ImageException {
public:
    ImageCreateError(std::string_view message) : 
        ImageException(message)
    { }
};

// Vulkan image bind exception
class ImageBindError : public ImageException {
public:
    ImageBindError(std::string_view message) : 
        ImageException(message)
    { }
};

/*
 *
 *      Command buffer exception
 *
 * */

// Generic command buffer exception
class CmdBufferException : public VulkanException {
public:
    CmdBufferException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan command buffer allocation exception
class CmdBufferAllocationError : public CmdBufferException {
public:
    CmdBufferAllocationError(std::string_view message) : 
        CmdBufferException(message)
    { }
};

// Vulkan command buffer begin exception
class CmdBufferBeginError : public CmdBufferException {
public:
    CmdBufferBeginError(std::string_view message) : 
        CmdBufferException(message)
    { }
};

// Vulkan command buffer end exception
class CmdBufferEndError : public CmdBufferException {
public:
    CmdBufferEndError(std::string_view message) : 
        CmdBufferException(message)
    { }
};

// Vulkan command buffer reset exception
class CmdBufferResetError : public CmdBufferException {
public:
    CmdBufferResetError(std::string_view message) : 
        CmdBufferException(message)
    { }
};

// Vulkan command buffer submit exception
class CmdBufferSubmitError : public CmdBufferException {
public:
    CmdBufferSubmitError(std::string_view message) : 
        CmdBufferException(message)
    { }
};

/*
 *
 *      Render pass exception
 *
 * */

// Generic render pass exception
class RenderPassException : public VulkanException {
public:
    RenderPassException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan command buffer submit exception
class RenderPassCreationError : public RenderPassException {
public:
    RenderPassCreationError(std::string_view message) : 
        RenderPassException(message)
    { }
};

/*
 *
 *      Render attachment exception
 *
 * */

// Generic render pass exception
class RenderAttachmentException : public VulkanException {
public:
    RenderAttachmentException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan command buffer submit exception
class RenderAttachmentCreationError : public RenderAttachmentException {
public:
    RenderAttachmentCreationError(std::string_view message) : 
        RenderAttachmentException(message)
    { }
};

/*
 *
 *      Swap chain exception
 *
 * */

// Generic swap chain exception
class SwapChainException : public VulkanException {
public:
    SwapChainException(std::string_view message) : 
        VulkanException(message)
    { }
};

// Vulkan swap chain initialization error exception
class SwapChainInitError : public SwapChainException {
public:
    SwapChainInitError(std::string_view message) : 
        SwapChainException(message)
    { }
};

// Vulkan swap chain image view error exception
class SwapChainViewError : public SwapChainException {
public:
    SwapChainViewError(std::string_view message) : 
        SwapChainException(message)
    { }
};

// Vulkan swap chain image acquisition error exception
class SwapChainImageAcquireError : public SwapChainException {
public:
    SwapChainImageAcquireError(std::string_view message) : 
        SwapChainException(message)
    { }
};

// Vulkan swap chain image presentation error exception
class SwapChainPresentError : public SwapChainException {
public:
    SwapChainPresentError(std::string_view message) : 
        SwapChainException(message)
    { }
};

} // namespace cndt::vulkan

#endif
