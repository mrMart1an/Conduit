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

// Vulkan command pool initialization error
class CommandPoolInitError : public InitializationError {
public:
    CommandPoolInitError(std::string_view message) : 
        InitializationError(message)
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
    DeviceMemoryError() : 
        DeviceException("Vulkan device memory error")
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
    BufferCreateError() : 
        BufferException("Vulkan buffer creation error")
    { }
};

// Vulkan buffer bind exception
class BufferBindError : public BufferException {
public:
    BufferBindError(std::string_view message) : 
        BufferException(message)
    { }
    BufferBindError() : 
        BufferException("Vulkan buffer bind error")
    { }
};

// Vulkan buffer map exception
class BufferMapError : public BufferException {
public:
    BufferMapError(std::string_view message) : 
        BufferException(message)
    { }
    BufferMapError() : 
        BufferException("Vulkan buffer map error")
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
    ImageCreateError() : 
        ImageException("Vulkan image creation error")
    { }
};

// Vulkan image bind exception
class ImageBindError : public ImageException {
public:
    ImageBindError(std::string_view message) : 
        ImageException(message)
    { }
    ImageBindError() : 
        ImageException("Vulkan image bind error")
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
    CmdBufferAllocationError() : 
        CmdBufferException("Vulkan command buffer allocation exception")
    { }
};

// Vulkan command buffer begin exception
class CmdBufferBeginError : public CmdBufferException {
public:
    CmdBufferBeginError(std::string_view message) : 
        CmdBufferException(message)
    { }
    CmdBufferBeginError() : 
        CmdBufferException("Vulkan command buffer begin exception")
    { }
};

// Vulkan command buffer end exception
class CmdBufferEndError : public CmdBufferException {
public:
    CmdBufferEndError(std::string_view message) : 
        CmdBufferException(message)
    { }
    CmdBufferEndError() : 
        CmdBufferException("Vulkan command buffer end exception")
    { }
};

// Vulkan command buffer reset exception
class CmdBufferResetError : public CmdBufferException {
public:
    CmdBufferResetError(std::string_view message) : 
        CmdBufferException(message)
    { }
    CmdBufferResetError() : 
        CmdBufferException("Vulkan command buffer reset exception")
    { }
};

// Vulkan command buffer submit exception
class CmdBufferSubmitError : public CmdBufferException {
public:
    CmdBufferSubmitError(std::string_view message) : 
        CmdBufferException(message)
    { }
    CmdBufferSubmitError() : 
        CmdBufferException("Vulkan command buffer submit exception")
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
    RenderPassCreationError() : 
        RenderPassException("Vulkan render pass creation exception")
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
    RenderAttachmentCreationError() : 
        RenderAttachmentException(
            "Vulkan render attachment creation exception"
        ) { }
};

} // namespace cndt::vulkan

#endif