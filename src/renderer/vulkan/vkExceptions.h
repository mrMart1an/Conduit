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
    DeviceException() : 
        VulkanException("Generic vulkan device error")
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

} // namespace cndt::vulkan

#endif
