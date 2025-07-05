#ifndef CNDT_VK_EXCEPTIONS
#define CNDT_VK_EXCEPTIONS

#include "conduit/renderer/rendererException.h"

#include <fmt/base.h>

namespace cndt::vulkan {

/*
 *
 *      Vulkan renderer exception type
 *
 * */

// Renderer generic exception
class VulkanException : public RendererException {
public:
    template<typename... Args>
    VulkanException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RendererException(
            RendererBackend::Vulkan,
            msg, std::forward<Args>(args)...
        )
    { }
};

// Unexpected vulkan error
class UnexpectedVkError : public VulkanException {
public:
    template<typename... Args>
    UnexpectedVkError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    InitializationError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan instance initialization error
class InstanceInitError : public InitializationError {
public:
    template<typename... Args>
    InstanceInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        InitializationError(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan surface initialization error
class SurfaceInitError : public InitializationError {
public:
    template<typename... Args>
    SurfaceInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        InitializationError(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan physical device initialization error
class PhysicalDeviceError : public InitializationError {
public:
    template<typename... Args>
    PhysicalDeviceError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        InitializationError(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan logical device initialization error
class DeviceInitError : public InitializationError {
public:
    template<typename... Args>
    DeviceInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        InitializationError(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    SyncObjectException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan fence initialization error
class FenceInitError : public SyncObjectException {
public:
    template<typename... Args>
    FenceInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SyncObjectException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan fence wait error
class FenceWaitError : public SyncObjectException {
public:
    template<typename... Args>
    FenceWaitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SyncObjectException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan fence reset error
class FenceResetError : public SyncObjectException {
public:
    template<typename... Args>
    FenceResetError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SyncObjectException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan semaphore initialization error
class SemaphoreInitError : public SyncObjectException {
public:
    template<typename... Args>
    SemaphoreInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SyncObjectException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    CommandPoolException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command pool initialization error
class CommandPoolInitError : public CommandPoolException {
public:
    template<typename... Args>
    CommandPoolInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CommandPoolException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command pool reset error
class CommandPoolResetError : public CommandPoolException {
public:
    template<typename... Args>
    CommandPoolResetError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CommandPoolException(msg, std::forward<Args>(args)...)
    { }
};

/*
 *
 *      Device exception
 *
 * */

// Generic Vulkan device exception
class DeviceException : public VulkanException {
public:
    template<typename... Args>
    DeviceException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan device memory allocation exception
class DeviceMemoryError : public DeviceException {
public:
    template<typename... Args>
    DeviceMemoryError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        DeviceException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    BufferException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan buffer creation exception
class BufferCreateError : public BufferException {
public:
    template<typename... Args>
    BufferCreateError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        BufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan buffer copy exception
class BufferCopyError : public BufferException {
public:
    template<typename... Args>
    BufferCopyError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        BufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan buffer map exception
class BufferMapError : public BufferException {
public:
    template<typename... Args>
    BufferMapError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        BufferException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    ImageException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan image creation exception
class ImageCreateError : public ImageException {
public:
    template<typename... Args>
    ImageCreateError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        ImageException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan image bind exception
class ImageBindError : public ImageException {
public:
    template<typename... Args>
    ImageBindError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        ImageException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    CmdBufferException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer allocation exception
class CmdBufferAllocationError : public CmdBufferException {
public:
    template<typename... Args>
    CmdBufferAllocationError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CmdBufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer begin exception
class CmdBufferBeginError : public CmdBufferException {
public:
    template<typename... Args>
    CmdBufferBeginError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CmdBufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer record exception
class CmdBufferRecordError : public CmdBufferException {
public:
    template<typename... Args>
    CmdBufferRecordError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CmdBufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer end exception
class CmdBufferEndError : public CmdBufferException {
public:
    template<typename... Args>
    CmdBufferEndError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CmdBufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer reset exception
class CmdBufferResetError : public CmdBufferException {
public:
    template<typename... Args>
    CmdBufferResetError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CmdBufferException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer submit exception
class CmdBufferSubmitError : public CmdBufferException {
public:
    template<typename... Args>
    CmdBufferSubmitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        CmdBufferException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    RenderPassException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan render pass creation exception
class RenderPassCreationError : public RenderPassException {
public:
    template<typename... Args>
    RenderPassCreationError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RenderPassException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan render pass incompatible attachment provided exception
class IncompatibleAttachmentsError : public RenderPassException {
public:
    template<typename... Args>
    IncompatibleAttachmentsError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RenderPassException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan render pass begin exception
class RenderPassBeginError : public RenderPassException {
public:
    template<typename... Args>
    RenderPassBeginError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RenderPassException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    RenderAttachmentException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan command buffer submit exception
class RenderAttachmentCreationError : public RenderAttachmentException {
public:
    template<typename... Args>
    RenderAttachmentCreationError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RenderAttachmentException(msg, std::forward<Args>(args)...)
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
    template<typename... Args>
    SwapChainException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan swap chain initialization error exception
class SwapChainInitError : public SwapChainException {
public:
    template<typename... Args>
    SwapChainInitError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SwapChainException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan swap chain image view error exception
class SwapChainViewError : public SwapChainException {
public:
    template<typename... Args>
    SwapChainViewError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SwapChainException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan swap chain image acquisition error exception
class SwapChainImageAcquireError : public SwapChainException {
public:
    template<typename... Args>
    SwapChainImageAcquireError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SwapChainException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan swap chain image presentation error exception
class SwapChainPresentError : public SwapChainException {
public:
    template<typename... Args>
    SwapChainPresentError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        SwapChainException(msg, std::forward<Args>(args)...)
    { }
};

/*
 *
 *      Shader module exception
 *
 * */

// Generic shader module exception
class ShaderModuleException : public VulkanException {
public:
    template<typename... Args>
    ShaderModuleException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan shader module file access error
class ShaderModuleFileError : public ShaderModuleException {
public:
    template<typename... Args>
    ShaderModuleFileError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        ShaderModuleException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan shader module creation error
class ShaderModuleCreateError : public ShaderModuleException {
public:
    template<typename... Args>
    ShaderModuleCreateError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        ShaderModuleException(msg, std::forward<Args>(args)...)
    { }
};

/*
 *
 *      Pipeline exception
 *
 * */

// Generic pipeline exception
class PipelineException : public VulkanException {
public:
    template<typename... Args>
    PipelineException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan pipeline create error
class PipelineCreationError : public PipelineException {
public:
    template<typename... Args>
    PipelineCreationError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        PipelineException(msg, std::forward<Args>(args)...)
    { }
};

/*
 *
 *      Descriptor set exception
 *
 * */

// Vulkan descriptor set general exception
class DescriptorSetException : public VulkanException {
public:
    template<typename... Args>
    DescriptorSetException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        VulkanException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan descriptor set layout creation error
class DescriptorSetLayoutBuildError : public DescriptorSetException {
public:
    template<typename... Args>
    DescriptorSetLayoutBuildError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        DescriptorSetException(msg, std::forward<Args>(args)...)
    { }
};

// Vulkan descriptor pool creation error
class DescriptorAllocatorError : public DescriptorSetException {
public:
    template<typename... Args>
    DescriptorAllocatorError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        DescriptorSetException(msg, std::forward<Args>(args)...)
    { }
};

} // namespace cndt::vulkan

#endif
