#ifndef CNDT_VK_EXCEPTIONS
#define CNDT_VK_EXCEPTIONS

#include "renderer/vulkan/vkRenderer.h"

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
} // namespace cndt::vulkan

#endif
